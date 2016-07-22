#include "hairloader.h"

#include <cmath>
#include <iostream>
#include <fstream>

static const double Pi = 4.0 * std::atan(1.0);

namespace {

void write_v(std::ofstream& os, const QVector3D& v) {
    os << "v " << v.x() << " "
               << v.y() << " "
               << v.z() << std::endl;
}

void write_vn(std::ofstream& os, const QVector3D& vn) {
    os << "vn " << vn.x() << " "
                << vn.y() << " "
                << vn.z() << std::endl;
}

void write_f(std::ofstream& os, int i, int j, int k) {
    os << "f " << i << " " << j << " " << k << std::endl;
}

}  // anonymous namespace

void Hair::saveMesh(const std::string& filename) const {
    static const int divs = 8;
    
    std::ofstream ofs(filename.c_str(), std::ios::out);
    std::vector<QVector3D> prev(divs, QVector3D(0.0f, 0.0f, 0.0f));
    std::vector<QVector3D> curr(divs, QVector3D(0.0f, 0.0f, 0.0f));

    int total = 0;
    int pos = 0;
    double radius = 0.05;
    
    for (int i = 0; i < segments.size(); i++) {
        int length = numSegments[i];
        const Segment* strand = &segments[pos];
        
        for (int j = 0; j < length; j++) {
            double tilt = 0.0;
            if (j != 0 && j != length - 1) {
                QVector3D u1 = (strand[j - 1].pos - strand[j].pos).normalized();
                QVector3D u2 = (strand[j + 1].pos - strand[j].pos).normalized();
                tilt = (Pi - std::acos(QVector3D::dotProduct(u1, u2)));
            }
            
            QVector3D v1, v2;
            if (j == 0) {
                v1 = strand[0].pos - strand[1].pos;
                v2 = strand[2].pos - strand[1].pos;
            } else if (j == length - 1) {
                v1 = strand[length - 3].pos - strand[length - 2].pos;
                v2 = strand[length - 1].pos - strand[length - 2].pos;
            } else {
                v1 = strand[j - 1].pos - strand[j].pos;
                v2 = strand[j + 1].pos - strand[j].pos;
            }
            
            v1 = v1.normalized();
            v2 = v2.normalized();
            
            QVector3D t = -v1;
            QVector3D u = QVector3D::crossProduct(v1, v2).normalized();
            QVector3D v = QVector3D::crossProduct(t, u).normalized();
            
            for (int k = 0; k < divs; k++) {
                double theta = 2.0 * Pi * k / divs;
                curr[k] = strand[j].pos + (u * std::cos(theta) +
                                           v * std::sin(theta) +
                                           t * std::sin(theta) * std::cos(tilt)) * radius;
            }
            
            // Save disk in the both ends of the hair.
            if (j == 0 || j == length - 1) {
                QVector3D nv = j == 0 ? -t : t;
                for (int k = 0; k < divs; k++) {
                    int l = (k + 1) % divs;
                    write_v(ofs, strand[j].pos);
                    write_v(ofs, curr[k]);
                    write_v(ofs, curr[l]);
                    write_vn(ofs, nv);
                    write_vn(ofs, nv);
                    write_vn(ofs, nv);
                    if (j == 0) {
                        write_f(ofs, total, total + 2, total + 1);
                    } else if (j == length - 1) {
                        write_f(ofs, total, total + 1, total + 2);
                    }
                    total += 3;
                }
            }
            
            // Save tube between neighboring segments.
            if (j != 0) {
                // Compute offset to minimize gap to prevent tube twist
                int offset = 0;
                double minLen = 1.0e12;
                for (int k = 0; k < divs; k++) {
                    double l = (prev[0] - curr[k]).lengthSquared();
                    if (minLen > l) {
                        minLen = l;
                        offset = k;
                    }
                }
                
                for (int k = 0; k < divs; k++) {
                    int l = (k + 1) % divs;
                    QVector3D p11 = prev[k];
                    QVector3D p12 = prev[l];
                    QVector3D p21 = curr[k];
                    QVector3D p22 = curr[l];
                    write_v(ofs, p11);
                    write_v(ofs, p12);
                    write_v(ofs, p21);
                    write_v(ofs, p22);
                    write_vn(ofs, (p11 - strand[j - 1].pos).normalized());
                    write_vn(ofs, (p12 - strand[j - 1].pos).normalized());
                    write_vn(ofs, (p21 - strand[j].pos).normalized());
                    write_vn(ofs, (p22 - strand[j].pos).normalized());
                    write_f(ofs, total, total + 1, total + 3);
                    write_f(ofs, total, total + 3, total + 2);
                    total += 4;
                }
            }
            
            for (int k = 0; k < divs; k++) {
                prev[k] = curr[k];
            }
        }
    }
    
    ofs.close();
}

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
