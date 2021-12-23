#ifndef _PHYSICS_ENGINE_H
#define _PHYSICS_ENGINE_H

#include "Utils.h"
#include "BinFileLoader.h"
#include <vector>
#include <iostream>
#include <string>
using namespace std;

//All force calculations
class PhysicsEngine {
	public:
		PhysicsEngine();
		void AddForcePoint(GlobalForce force);
		void Render(BinFileLoader& file, Vec3* Positions, int pntr, int FrameRate, float Seconds);
	private:
		vector<GlobalForce> GBLForces;
};

#endif