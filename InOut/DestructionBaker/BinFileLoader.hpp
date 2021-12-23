#ifndef _BINFILELOADER_H
#define _BINFILELOADER_H
#include "Utils.h"
#include <iostream>
#include <fstream>
#include <vector>
using namespace std;

//Binary converter (comparative to c# BitConverter class)
static class BinConverter {
    public:
        static float BytesToFloat(vector<byte> v, int o);
        static int BytesToInt(vector<byte> v, int o);
        static Vec3 BytesToVec3(vector<byte> v, int o);
};
//General parsing to load .bin file
class BinFileLoader {
    public:
        BinFileLoader(string path, bool Reading);
        ArrayPointer FindStringID(string id);
        Vec3* ReadVec3Array(ArrayPointer ptr);
        int* ReadIntArray(ArrayPointer ptr);

        void WriteVec3Array(Vec3* data, int length);
        void WriteToFile();

        ofstream FileWriter;
    private:
        string FilePath;
        vector<byte> FileData;
        const char* WriteBuffer;
};

#endif