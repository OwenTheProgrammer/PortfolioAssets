#include "PhysicsEngine.h"

PhysicsEngine::PhysicsEngine() {}

//Add force point to the global forces
void PhysicsEngine::AddForcePoint(GlobalForce force) {GBLForces.push_back(force);}

//Bake the physics
void PhysicsEngine::Render(BinFileLoader& file, Vec3* Positions, int pntr, int FrameRate, float Seconds) {
	//General time calculations
	printf("Baking:\nFrameRate: %i, Seconds: %f\n", FrameRate, Seconds);
	int RenderPass = (FrameRate * Seconds);
	float timeStep = Seconds / (float)RenderPass;

	int objCount = pntr / 12;

	//Compute the physics over time
	Vec3* Trajectories = new Vec3[objCount];
	Vec3* Displacements = new Vec3[objCount];
	for (int i = 0; i < objCount; i++) {
		//Add some randomness to the angular velocity vectors
		Vec3 random = Vec3(((float)(rand()%100)/100.0)*2.0-1.0);
		//Bring strength of vector into the mix
		Vec3 init = GBLForces[1].Compose();
		//Normalize the strength
		init.y /= GBLForces[1].Strength;
		//Rotate the coordinate space for unity (blender z+ up, unity y+ up)
		Vec3 f = Vec3(Positions[i].x, Positions[i].z, Positions[i].y);
		//Get facing direction vector from force
		Vec3 dist = (f - GBLForces[2].Vector);
		//Make a pointing vector
		dist.Normalize();
		//Invert it since the coords are rotated
		dist.x *= -1.0; dist.z *= -1.0;
		//Weight 75% to explosion 25% to random
		Vec3 Displacement = ((dist * 0.75) + (random * 0.25)) + init;
		//Weight more gravity to x and y
		Displacement *= Vec3(1.2, 1.2, 0.8);
		Trajectories[i] = Displacement;
		Displacements[i] = Vec3();
	}

	//Export Frame Count
	string RenderPassLabel = "FC:";
	file.FileWriter.clear();
	file.FileWriter.write(RenderPassLabel.c_str(), RenderPassLabel.length());
	file.FileWriter.write((char*)&RenderPass, sizeof(int));
	//Export Time
	string SecondsLabel = "TIME:";
	file.FileWriter.write(SecondsLabel.c_str(), SecondsLabel.length());
	file.FileWriter.write((char*)&Seconds, sizeof(float));
	//Export Rate
	string RateLabel = "RATE:";
	file.FileWriter.write(RateLabel.c_str(), RateLabel.length());
	file.FileWriter.write((char*)&FrameRate, sizeof(int));
	file.FileWriter.flush();

	//Calculate the simulation
	printf("Baking %i frames step: %f\n", RenderPass, timeStep);
	float f = 0.0;
	for (int i = 0; i < RenderPass; i++, f+=timeStep) {
		printf("Calculating Step %i / %i (t: %f)\n", i, RenderPass, f);
		string Label = "FRM" + to_string(i) + ":";
		file.FileWriter.clear();
		file.FileWriter.write(Label.c_str(), Label.length());
		file.FileWriter.write((char*)&pntr, sizeof(int));

		//Calculate and write to the bin output (frame one is nothing)
		for (int j = 0; j < objCount; j++) {
			if (i > 1) {
				Trajectories[j].x += GBLForces[0].Vector.x * timeStep;
				Trajectories[j].y += GBLForces[0].Vector.y * timeStep;
				Trajectories[j].z += GBLForces[0].Vector.z * timeStep;
				Displacements[j].x += Trajectories[j].x;
				Displacements[j].y += Trajectories[j].y;
				Displacements[j].z += Trajectories[j].z;
			}
			file.FileWriter.write((char*)&Displacements[j].x, sizeof(float));
			file.FileWriter.write((char*)&Displacements[j].y, sizeof(float));
			file.FileWriter.write((char*)&Displacements[j].z, sizeof(float));
		}
		file.FileWriter.flush();
	}
	file.WriteToFile();
}