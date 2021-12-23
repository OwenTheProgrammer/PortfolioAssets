#include "Utils.h"
//Just some simple math and computation
ArrayPointer::ArrayPointer() : pntr(-1), length(0) {}
ArrayPointer::ArrayPointer(int p, int l) : pntr(p), length(l) {}

//Vector3 Constructors
Vec3::Vec3() : x(0.0), y(0.0), z(0.0) {}
Vec3::Vec3(float v) : x(v), y(v), z(v) {}
Vec3::Vec3(float a, float b, float c) : x(a), y(b), z(c) {}
Vec3::Vec3(float val[]) : x(val[0]), y(val[1]), z(val[2]) {}

//Normalize this vector
void Vec3::Normalize() {
    float d = sqrtf(powf(this->x, 2) + powf(this->y, 2) + powf(this->z, 2));
    this->x /= d;this->y /= d;this->z /= d;
}

//Return a vector as normalized
Vec3 Vec3::Normalized() {
    float s = sqrtf(powf(this->x, 2) + powf(this->y, 2) + powf(this->z, 2));
    return Vec3(this->x / s, this->y / s, this->z / s);
}

GlobalForce::GlobalForce() : Vector(Vec3()), Strength(0.0) {}
GlobalForce::GlobalForce(Vec3 v, float s) : Vector(v), Strength(s) {}

//Add strength to vector for convenience
Vec3 GlobalForce::Compose() {return Vec3(Vector.x * Strength, Vector.y * Strength, Vector.z * Strength);}

//Debug all data given
void PrintBinFileData(ArrayPointer VertexBlockPtr, ArrayPointer OffsetBlockPtr, Vec3* Positions, int* VertPntrs) {
    printf("VERT:\n");
    for (int i = 0; i < VertexBlockPtr.length / 12; i++) {
        printf("Vertex: <%f, %f, %f>\n", Positions[i].x, Positions[i].y, Positions[i].z);
    }
    printf("OFS:\n");
    for (int i = 0; i < OffsetBlockPtr.length / 4; i++) {
        printf("offset: %i \n", VertPntrs[i]);
    }
}