#ifdef _MSC_VER
#pragma once
#endif

#ifndef _HAIR_LOADER_H_
#define _HAIR_LOADER_H_

#include <string>
#include <QtGui/qvector3d.h>

enum class RefModel : int {
    KajiyaKay = 0x01,
    Marschner = 0x02
};

struct Segment {
    QVector3D pos;
    QVector3D tangent;    
};

struct Hair {
    std::vector<unsigned short> numSegments;
    std::vector<Segment> segments;
    QVector3D diffuse = QVector3D(0.9f, 0.8f, 0.5f);
    QVector3D specular = QVector3D(0.3f, 0.3f, 0.3f);
    RefModel refmodel = RefModel::KajiyaKay;
};

bool loadHair(const std::string& filename, Hair* hair);

#endif  // _HAIR_LOADER_H_
