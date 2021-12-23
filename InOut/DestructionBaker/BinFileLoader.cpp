#include "BinFileLoader.h"

//remove unsafe C library function warnings
#pragma warning(disable : 4996)

//Converts 4 bytes to a float
float BinConverter::BytesToFloat(vector<byte> v, int o) {
    data_float val;for(int i=0;i<4;i++)val.buffer[i]=v[i+o];
    return val.value;
}

//Converts 4 bytes to an integer
int BinConverter::BytesToInt(vector<byte> v, int o) {
    data_int val; for(int i=0;i<4;i++)val.buffer[i]=v[i+o];
    return val.value;
}

//Converts 4*3 bytes to a Vector3
Vec3 BinConverter::BytesToVec3(vector<byte> v, int o) {
    data_float val[3]; float xyz[3];
    for(int i = 0; i < 3; i++) {
        for (int j = 0; j < 4; j++) {
            val[i].buffer[j] = v[(i*4)+j+o];
        } xyz[i] = val[i].value;
    } return Vec3(xyz);
}

//Read from the file / Write to the file
BinFileLoader::BinFileLoader(string path, bool Reading) {
    FilePath = path;
    if (Reading) {
        printf("Loading File: %s\n", path.c_str());
        //Optimized reading for binary values
        ifstream file(path, ios::binary);
        file.unsetf(ios::skipws);
        file.seekg(0, ios::end);
        streampos fsize = file.tellg();
        file.seekg(0, ios::beg);
        FileData.reserve(fsize);
        FileData.insert(FileData.begin(),
            istream_iterator<byte>(file),
            istream_iterator<byte>());
        file.close();
        printf("Loaded %i bytes\n", FileData.size());
    }
    else {
        FileWriter = ofstream(FilePath, ios::out|ios::binary);
    }
}

//implementation of String.Find();
ArrayPointer BinFileLoader::FindStringID(string id) {
    const char* working = id.c_str();
    printf("Finding ID: '%s'\n", working);
    int KernLength = id.length();
    for(int i = 0; i < FileData.size(); i++)
        if (FileData[i] == ':') {
            bool KernelPass = true;
            for (int k = 1; k < KernLength+1 && KernelPass; k++)
                KernelPass = (FileData[i-k]==working[KernLength-k]);
            if (KernelPass) {
                int length = BinConverter::BytesToInt(FileData, i + 1);
                printf("%s: offset %i, length %i bytes\n", working, i, length);
                return ArrayPointer(i, length);
            }
        }
    return ArrayPointer();
}

//Read data at point in file as Vector3
Vec3* BinFileLoader::ReadVec3Array(ArrayPointer ptr) {
    int blkSize = ptr.length / sizeof(Vec3);
    Vec3* data = new Vec3[blkSize];
    for (int i = 0, p = ptr.pntr + 5; p < (ptr.pntr+ptr.length + 5); p+=sizeof(Vec3), i++) {
        data[i] = BinConverter::BytesToVec3(FileData, p);
    } return data;
}

//Read data at point in file as int[]
int* BinFileLoader::ReadIntArray(ArrayPointer ptr) {
    int blkSize = ptr.length / sizeof(int);
    int* data = new int[blkSize];
    for (int i = 0, p = ptr.pntr + 5; p < (ptr.pntr + ptr.length + 5); p += sizeof(int), i++) {
        data[i] = BinConverter::BytesToInt(FileData, p);
    } 
    return data;
}

//Write data at point in file as Vector3[]
void BinFileLoader::WriteVec3Array(Vec3* data, int length) {
    for (int i = 0; i < length; i++) {
        const char* x = reinterpret_cast<const char*>(&data[i].x);
        const char* y = reinterpret_cast<const char*>(&data[i].y);
        const char* z = reinterpret_cast<const char*>(&data[i].z);

        FileWriter.write((const char*)x, sizeof(float));
        FileWriter.write((const char*)y, sizeof(float));
        FileWriter.write((const char*)z, sizeof(float));
    }
    FileWriter.flush();
}

//Close the file for usage with constructor
void BinFileLoader::WriteToFile() {
    FileWriter.flush();
    FileWriter.close();
}