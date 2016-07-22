#include "hairloader.h"

#include <iostream>
#include <fstream>

bool loadHair(const std::string& filename, Hair* hair) {
    std::vector<QVector3D> positions;
    
    std::ifstream ifs(filename.c_str(), std::ios::in | std::ios::binary);
    if (!ifs.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return false;
    }

    char token[8] = { 0 };
    ifs.read(token, sizeof(char) * 4);
    printf("%s\n", token);

    int nStrands;
    ifs.read((char*)&nStrands, sizeof(int));
    printf("Strands: %d\n", nStrands);

    int nTotal;
    ifs.read((char*)&nTotal, sizeof(int));
    printf("Total: %d\n", nTotal);

    int flags;
    ifs.read((char*)&flags, sizeof(int));

    int nSegments;
    ifs.read((char*)&nSegments, sizeof(int));
    if (((flags >> 0) & 0x01) != 0) {
        printf("Segments: %d\n", nSegments);
    }

    float thickness;
    ifs.read((char*)&thickness, sizeof(float));
    if (((flags >> 2) & 0x01) != 0) {
        printf("Thickness: %f\n", thickness);
    }

    float transparency;
    ifs.read((char*)&transparency, sizeof(float));
    if (((flags >> 3) & 0x01) != 0) {
        printf("Transparency: %f\n", transparency);
    }

    float rgb[3];
    ifs.read((char*)rgb, sizeof(float) * 3);
    if (((flags >> 4) & 0x01) != 0) {
        printf("Color: %f %f %f\n", rgb[0], rgb[1], rgb[2]);
        hair->diffuse = QVector3D(rgb[0], rgb[1], rgb[2]);
    }

    char msg[128] = { 0 };
    ifs.read(msg, sizeof(char) * 88);
    printf("%s\n", msg);

    hair->numSegments.resize(nStrands);
    ifs.read((char*)&hair->numSegments[0], sizeof(unsigned short) * nStrands);

    positions.resize(nTotal);
    ifs.read((char*)&positions[0], sizeof(float) * 3 * nTotal);

    ifs.close();

    hair->segments.resize(nTotal);
    for (int i = 0; i < nTotal; i++) {
        QVector3D tangent;
        if (i == 0) {
            tangent = positions[i + 1] - positions[i];
        } else if (i == nTotal - 1) {
            tangent = positions[i] - positions[i - 1];
        } else {
            tangent = 0.5f * (positions[i + 1] - positions[i - 1]);
        }

        hair->segments[i].pos = positions[i];
        hair->segments[i].tangent = tangent;
    }

    return true;
}