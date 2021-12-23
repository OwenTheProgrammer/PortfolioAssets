#include <iostream>
#include <fstream>
#include <vector>
#include "Utils.h"
#include "BinFileLoader.h"
#include "PhysicsEngine.h"
using namespace std;

//Filepath to input file
string BinFilePath("T:/TrophyTheElephantAnimation/BridgeMeshData.bin");

int main() {
    BinFileLoader file = BinFileLoader(BinFilePath);
    //Find vertex and triangle offset pointers in file
    ArrayPointer VertexBlockPtr = file.FindStringID("vert");
    ArrayPointer OffsetBlockPtr = file.FindStringID("ofs");

    //Load the data
    Vec3* Positions = file.ReadVec3Array(VertexBlockPtr);
    int* VertPntrs = file.ReadIntArray(OffsetBlockPtr);
    
    //Create forces for Physics engine
    GlobalForce WorldForce = GlobalForce(Vec3(0, -10, 0), 0.4);
    GlobalForce Explosion = GlobalForce(Vec3(0, 17.93, 0), 10);
    GlobalForce InitialForce = GlobalForce(Vec3(0, 1, 0), 2);
    
    PhysicsEngine engine = PhysicsEngine(WorldForce);
    engine.AddForcePoint(Explosion);
    engine.AddForcePoint(InitialForce);

    //Log all data points
    PrintBinFileData(VertexBlockPtr, OffsetBlockPtr, Positions, VertPntrs);

    return 0;
}