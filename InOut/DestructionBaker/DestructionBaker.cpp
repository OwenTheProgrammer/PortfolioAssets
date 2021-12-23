#include <iostream>
#include <fstream>
#include <vector>
#include "Utils.h"
#include "BinFileLoader.h"
#include "PhysicsEngine.h"
using namespace std;

int main() {
    //Input for release exe
    string BinFilePath;
    string OutFilePath;
    printf("Input .bin Path: ");
    getline(cin, BinFilePath);
    printf("\nOutput Path: ");
    getline(cin, OutFilePath);

    //Load the file
    BinFileLoader file = BinFileLoader(BinFilePath, true);
    //find the "vert, ofs:" tags
    ArrayPointer VertexBlockPtr = file.FindStringID("vert");
    ArrayPointer OffsetBlockPtr = file.FindStringID("ofs");

    //Parse bin to arrays
    Vec3* Positions = file.ReadVec3Array(VertexBlockPtr);
    int* VertPntrs = file.ReadIntArray(OffsetBlockPtr);

    GlobalForce WorldForce = GlobalForce(Vec3(0, -9.81, 0), 10); // gravity
    GlobalForce InitialForce = GlobalForce(Vec3(0, 1, 0), 2); // init
    GlobalForce Explosion = GlobalForce(Vec3(0, 6, 0), 5); // explosion

    BinFileLoader output = BinFileLoader(OutFilePath, false);
    PhysicsEngine engine = PhysicsEngine();
    engine.AddForcePoint(WorldForce);
    engine.AddForcePoint(InitialForce);
    engine.AddForcePoint(Explosion);

    engine.Render(output, Positions, VertexBlockPtr.length, 30, 8.0);

    return 0;
}