#ifndef _UTILS_H
#define _UTILS_H

#include <stdio.h>
#include <math.h>

typedef unsigned char byte;
union data_float { byte buffer[4]; float value; }; // Re-interpret bytes to float
union data_int { byte buffer[4]; int value; }; // Re-interpret bytes to int

//Simple structure for the bin format pointing
struct ArrayPointer {
    int pntr, length;

    ArrayPointer();
    ArrayPointer(int p, int l);
};

//Vector3 implementation
struct Vec3 {
    float x, y, z;

    Vec3();
    Vec3(float v);
    Vec3(float a, float b, float c);
    Vec3(float val[]);

    void Normalize();
    Vec3 Normalized();

    //Vector3 operators
    Vec3 operator -(Vec3 b) {
        return Vec3(this->x - b.x, this->y - b.y, this->z - b.z);
    }
    Vec3 operator +(Vec3 b) {
        return Vec3(this->x + b.x, this->y + b.y, this->z + b.z);
    }
    Vec3 operator *(float v) {
        return Vec3(this->x * v, this->y * v, this->z * v);
    }
    Vec3 operator +=(Vec3 a) {
        return Vec3(this->x + a.x, this->y + a.y, this->z + a.z);
    }
    Vec3 operator *=(Vec3 a) {
        return Vec3(this->x * a.x, this->y * a.y, this->z * a.z);
    }
};

struct GlobalForce {
    Vec3 Vector;
    float Strength;

    GlobalForce();
    GlobalForce(Vec3 v, float s);

    Vec3 Compose();
};

void PrintBinFileData(ArrayPointer VertexBlockPtr, ArrayPointer OffsetBlockPtr, Vec3* Positions, int* VertPntrs);

#endif