#ifndef QUALITY_H
#define QUALITY_H

#include <iostream>
#include <QVector3D>
#include <QPoint>

class Quality
{
public :
    float AspectRatio1 (std::vector<Face>& faces);
    float AspectRatio2 (std::vector<Face>& faces);
    float AspectRatio3 (std::vector<Face>& faces);
    static const float maximum(const float x, const float y, const float z);
    static const float minimum(const float x, const float y, const float z);
    void contours (std::vector<Edge>& edges, std::vector<Edge>& f, std::vector<Edge>& nonVariety);
    float distancePointToPoint(QVector3D &pos1, QVector3D &pos2);
    QVector3D vectorPointToPoint(QVector3D &p1, QVector3D &p2);
    float distancePointToRay(QVector3D& origin, QVector3D& dir, QVector3D& point);

private :

};

#endif // QUALITY_H
