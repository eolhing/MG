#include "vefmodel.hpp"
#include "octree.hpp"
#include "math.h"
#include "quality.hpp"
#include <cmath>
#include <limits>

void analyseArray(std::vector<float>& data, float& mean, float& sd, float& _min, float& _max)
{
    mean = 0.0;
    sd = 0.0;
    _max = std::numeric_limits<float>::min();
    _min = std::numeric_limits<float>::max();

    for(std::vector<float>::iterator it = data.begin(); it != data.end(); ++it)
    {
        mean += *it;
        _min = fmin(*it, _min);
        _max = fmax(*it, _max);
    }
    mean = mean / data.size();

    for(std::vector<float>::iterator it = data.begin(); it != data.end(); ++it)
        sd += (*it - mean) * (*it - mean);
    sd = sqrt(sd / data.size());
}

void Quality::MWA(std::vector<Vertex*>& vertices)
{
    std::vector<Vertex*>::iterator beg = vertices.begin();
    std::vector<Vertex*>::iterator end = vertices.end();

    for( ; beg != end; ++beg){
        std::vector<Face*>::iterator beg_faces = (*beg)->faces.begin();
        std::vector<Face*>::iterator end_faces = (*beg)->faces.end();

        float3 Nmwa;
        Nmwa.x = Nmwa.y = Nmwa.z = 0.f;
        unsigned int cpt = 0;
        for(  ;beg_faces != end_faces; ++beg_faces, ++cpt){
            Edge *e1, *e2;
            e1 = e2 = NULL;

            if( (*beg_faces)->edges[0]->v1 == (*beg) || (*beg_faces)->edges[0]->v2 == (*beg) ) e1 = (*beg_faces)->edges[0];
            if( (*beg_faces)->edges[1]->v1 == (*beg) || (*beg_faces)->edges[1]->v2 == (*beg)){
                if(e1 == NULL) e1 = (*beg_faces)->edges[1];
                else e2 = (*beg_faces)->edges[1];
            }

            if( (*beg_faces)->edges[2]->v1 == (*beg) || (*beg_faces)->edges[2]->v2 == (*beg)) e2 = (*beg_faces)->edges[2];

            float3 v_e1, v_e2;
            v_e1.x = e1->v2->x - e1->v1->x;
            v_e1.y = e1->v2->y - e1->v1->y;
            v_e1.z = e1->v2->z - e1->v1->z;

            v_e2.x = e2->v2->x - e2->v1->x;
            v_e2.y = e2->v2->y - e2->v1->y;
            v_e2.z = e2->v2->z - e2->v1->z;

            float dotE1E2 = v_e1.x * v_e2.x + v_e1.y * v_e2.y + v_e1.z * v_e2.z;

            float angleE1E2 = static_cast<float>(acos(dotE1E2 / sqrt(v_e1.x * v_e1.x + v_e1.y * v_e1.y + v_e1.z * v_e1.z) * sqrt(v_e2.x * v_e2.x + v_e2.y * v_e2.y + v_e2.z * v_e2.z)));
            Nmwa.x += angleE1E2 * (*beg_faces)->nx;
            Nmwa.y += angleE1E2 * (*beg_faces)->ny;
            Nmwa.z += angleE1E2 * (*beg_faces)->nz;

        }
        Nmwa.x /= static_cast<float>(cpt);
        Nmwa.y /= static_cast<float>(cpt);
        Nmwa.z /= static_cast<float>(cpt);
        cpt = 0;
    }

}

void Quality::MWRELR(std::vector<Vertex *> &vertices)
{
    std::vector<Vertex*>::iterator beg = vertices.begin();
    std::vector<Vertex*>::iterator end = vertices.end();

    for( ; beg != end; ++beg){
        std::vector<Face*>::iterator beg_faces = (*beg)->faces.begin();
        std::vector<Face*>::iterator end_faces = (*beg)->faces.end();

        float3 Nmwa;
        Nmwa.x = Nmwa.y = Nmwa.z = 0.f;
        unsigned int cpt = 0;
        for(  ;beg_faces != end_faces; ++beg_faces, ++cpt){
            Edge *e1, *e2;
            e1 = e2 = NULL;

            if( (*beg_faces)->edges[0]->v1 == (*beg) || (*beg_faces)->edges[0]->v2 == (*beg) ) e1 = (*beg_faces)->edges[0];
            if( (*beg_faces)->edges[1]->v1 == (*beg) || (*beg_faces)->edges[1]->v2 == (*beg)){
                if(e1 == NULL) e1 = (*beg_faces)->edges[1];
                else e2 = (*beg_faces)->edges[1];
            }

            if( (*beg_faces)->edges[2]->v1 == (*beg) || (*beg_faces)->edges[2]->v2 == (*beg)) e2 = (*beg_faces)->edges[2];

            float3 v_e1, v_e2;
            v_e1.x = e1->v2->x - e1->v1->x;
            v_e1.y = e1->v2->y - e1->v1->y;
            v_e1.z = e1->v2->z - e1->v1->z;

            v_e2.x = e2->v2->x - e2->v1->x;
            v_e2.y = e2->v2->y - e2->v1->y;
            v_e2.z = e2->v2->z - e2->v1->z;

            float lengthE1, lengthE2;
            lengthE1 = sqrtf(v_e1.x * v_e1.x + v_e1.y * v_e1.y + v_e1.z * v_e1.z);
            lengthE2 = sqrtf(v_e2.x * v_e2.x + v_e2.y * v_e2.y + v_e2.z * v_e2.z);

            float sqrtE1E2 = sqrtf(lengthE1 * lengthE2);

            Nmwa.x += (*beg_faces)->nx / sqrtE1E2;
            Nmwa.y += (*beg_faces)->ny / sqrtE1E2;
            Nmwa.z += (*beg_faces)->nz / sqrtE1E2;

        }
        Nmwa.x /= static_cast<float>(cpt);
        Nmwa.y /= static_cast<float>(cpt);
        Nmwa.z /= static_cast<float>(cpt);
        cpt = 0;
    }

}

void Quality::normaleComputation(std::vector<Face*>& faces)
{
    std::vector<Face*>::iterator beg = faces.begin();
    std::vector<Face*>::iterator end = faces.end();



    for(; beg != end; ++beg){
        Edge* e1 = (*beg)->edges[0];
        Edge* e2 = (*beg)->edges[1];
        float3 v1,v2;
        v1.x = e1->v2->x - e1->v1->x;
        v1.y = e1->v2->y - e1->v1->y;
        v1.z = e1->v2->z - e1->v1->z;

        v2.x = e2->v2->x - e2->v1->x;
        v2.y = e2->v2->y - e2->v1->y;
        v2.z = e2->v2->z - e2->v1->z;

        float3 normale;
        normale.x = v1.y * v2.z - v1.z * v2.y;
        normale.y = v1.x * v2.z - v1.z * v2.x;
        normale.z = v1.x * v2.y - v2.x * v1.y;
        normale.x = -normale.x;
        normale.y = -normale.y;
        normale.z = -normale.z;

        (*beg)->nx = normale.x;
        (*beg)->ny = normale.y;
        (*beg)->nz = normale.z;

    }
}

float Quality::getAspectRatio1(const std::vector<Face*>& faces, float& ratioMean, float& ratioSD, float& ratioMin, float& ratioMax)
{
    QVector3D p1, p2, p3;
    float distp1p2, distp1p3, distp2p3;
    float distBase, distPoint;
    QVector3D vecBase;

    std::vector<float> ratios;
    ratios.resize(faces.size());

    // Extract all ratios into data
    for (unsigned int i = 0; i < faces.size(); ++i)
    {
        if (faces[i]->vertices.size() == 3)
        {
            p1 = QVector3D(faces[i]->vertices[0]->x, faces[i]->vertices[0]->y, faces[i]->vertices[0]->z);
            p2 = QVector3D(faces[i]->vertices[1]->x, faces[i]->vertices[1]->y, faces[i]->vertices[1]->z);
            p3 = QVector3D(faces[i]->vertices[2]->x, faces[i]->vertices[2]->y, faces[i]->vertices[2]->z);

            distp1p2 = distancePointToPoint(p1, p2);
            distp1p3 = distancePointToPoint(p1, p3);
            distp2p3 = distancePointToPoint(p2, p3);

            if (distp1p2 > distp1p3 && distp1p2 > distp2p3)
            {
                    distBase = distp1p2;
                    vecBase = vectorPointToPoint(p1, p2);
                    distPoint = Quality::distancePointToRay(p1, vecBase, p3);
            }
            else if (distp1p3 > distp1p2 && distp1p3 > distp2p3)
            {
                    distBase = distp1p3;
                    vecBase = vectorPointToPoint(p1, p3);
                    distPoint = Quality::distancePointToRay(p3, vecBase, p2);
            }
            else
            {
                distBase = distp1p3;
                vecBase = vectorPointToPoint(p1, p3);
                distPoint = Quality::distancePointToRay(p3, vecBase, p2);
            }

            ratios[i] = (distPoint / distBase);
        }
    }

    analyseArray(ratios, ratioMean, ratioSD, ratioMin, ratioMax);
    return ratioMean;
}

float Quality::getAspectRatio2(const std::vector<Face*> &faces, float& ratioMean, float& ratioSD, float& ratioMin, float& ratioMax)
{
    QVector3D p1,p2,p3;
    float distp1p2, distp1p3, distp2p3;
    float s;

    std::vector<float> ratios;
    ratios.resize(faces.size());

    for (unsigned int i = 0; i < faces.size(); ++i)
    {
        if (faces[i]->vertices.size() == 3)
        {
            p1 = QVector3D(faces[i]->vertices[0]->x, faces[i]->vertices[0]->y, faces[i]->vertices[0]->z);
            p2 = QVector3D(faces[i]->vertices[1]->x, faces[i]->vertices[1]->y, faces[i]->vertices[1]->z);
            p3 = QVector3D(faces[i]->vertices[2]->x, faces[i]->vertices[2]->y, faces[i]->vertices[2]->z);

            distp1p2 = distancePointToPoint(p1, p2);
            distp1p3 = distancePointToPoint(p1, p3);
            distp2p3 = distancePointToPoint(p2, p3);

            s = (distp1p2 + distp1p3 + distp2p3) / 2.f;
            ratios[i] = (distp1p2 * distp1p3 * distp2p3) / (8.f * (s - distp1p2) * (s - distp1p3) * (s - distp2p3));
        }
    }

    analyseArray(ratios, ratioMean, ratioSD, ratioMin, ratioMax);
    return ratioMean;
}

float Quality::getAspectRatio3(const std::vector<Face*> &faces, float& ratioMean, float& ratioSD, float& ratioMin, float& ratioMax)
{

    QVector3D p1;
    QVector3D p2;
    QVector3D p3;

    float distp1p2; //a
    float distp1p3; //b
    float distp2p3; //c

    float edgeMax = 0.f;
    float edgeMin = 0.f;

    std::vector<float> ratios;
    ratios.resize(faces.size());

    for (unsigned int i = 0; i < faces.size(); ++i)
    {
        if (faces[i]->vertices.size() == 3)
        {
            p1 = QVector3D(faces[i]->vertices[0]->x, faces[i]->vertices[0]->y, faces[i]->vertices[0]->z);
            p2 = QVector3D(faces[i]->vertices[1]->x, faces[i]->vertices[1]->y, faces[i]->vertices[1]->z);
            p3 = QVector3D(faces[i]->vertices[2]->x, faces[i]->vertices[2]->y, faces[i]->vertices[2]->z);

            distp1p2 = distancePointToPoint(p1, p2);
            distp1p3 = distancePointToPoint(p1, p3);
            distp2p3 = distancePointToPoint(p2, p3);

            edgeMax = std::max(std::max(distp1p2, distp1p3), distp2p3);
            edgeMin = std::min(std::min(distp1p2, distp1p3), distp2p3);

            ratios[i] = edgeMax / edgeMin;
        }
        else
        {
            std::cerr << "invalid face : more or less than 3 edges" << std::endl;
        }
    }

    analyseArray(ratios, ratioMean, ratioSD, ratioMin, ratioMax);
    return ratioMean;
}

float Quality::getDegree(const std::vector<Vertex*> &vertices, float& degreeMean, float& degreeSD, float& degreeMin, float& degreeMax)
{
    std::vector<float> degrees;
    degrees.resize(vertices.size());

    for (unsigned int i = 0; i < vertices.size(); ++i)
        degrees[i] = vertices[i]->edges.size();

    analyseArray(degrees, degreeMean, degreeSD, degreeMin, degreeMax);
    return degreeMean;
}

Edge* getNextEdge (std::vector<Edge*> edges, Edge* edge, std::vector<Edge*>& edgesVisited)
{
     for (unsigned int i = 0; i < edges.size(); ++i)
     {
         if(std::find(edgesVisited.begin(), edgesVisited.end(), edges[i]) == edgesVisited.end()) // If edge is not in the visited list
         {
             if(edge != edges[i])
             {
                 if (edge->v2 == edges[i]->v1 || edge->v2 == edges[i]->v2 || edge->v1 == edges[i]->v1 || edge->v1 == edges[i]->v2)
                     return edges[i];
             }
         }
     }
     return NULL;
}

int Quality::getHoleCount(std::vector<Edge*>& edges)
{
    int cmp = 0;

    std::vector<Edge*> e;
    Edge* nextEdge;

    for (unsigned int i = 1; i < edges.size(); ++i)
    {
        if(std::find(e.begin(), e.end(), edges[i]) == e.end()) // If edge is not in the visited list
        {
            nextEdge = getNextEdge(edges, edges[i], e);

            while (nextEdge != NULL)
            {
                e.push_back(nextEdge);
                nextEdge = getNextEdge(edges, nextEdge, e);
            }

            cmp++;
        }
    }

    return cmp;
}

void Quality::extractBoudaries (std::vector<Edge*>& edges, std::vector<Edge*>& boundaries, std::vector<Edge>& nonManifold)
{
    for (unsigned int i = 0; i < edges.size(); ++i)
    {
        if (edges[i]->faces.size() == 1)
        {
            boundaries.push_back(edges[i]);
            edges[i]->v1->r = 1.f;
            edges[i]->v1->g = edges[i]->v1->b = 0.f;
        }
        else if (edges[i]->faces.size() > 2)
        {
            nonManifold.push_back(*(edges[i]));
        }
    }
}

float Quality::distancePointToPoint(const QVector3D &pos1, const QVector3D &pos2)
{
    return sqrt((pos1.x() - pos2.x()) * (pos1.x() - pos2.x()) + (pos1.y() - pos2.y()) * (pos1.y() - pos2.y()) + (pos1.z() - pos2.z()) * (pos1.z() - pos2.z()));
}

QVector3D Quality::vectorPointToPoint(const QVector3D &p1, const QVector3D &p2)
{
    return QVector3D(p2.x() - p1.x(), p2.y() - p1.y(), p2.z() - p1.z());
}

float Quality::distancePointToRay(const QVector3D& origin, const QVector3D& dir, const QVector3D& point)
{
    QVector3D dirToPoint = QVector3D(point.x() - origin.x(), point.y() - origin.y(), point.z() - origin.z());

    float scalarP = dir.x() * dirToPoint.x() +  dir.y() * dirToPoint.y() + dir.z() * dirToPoint.z();
    float norm = sqrt(dir.x() * dir.x() + dir.y() * dir.y() + dir.z() * dir.z()) *
                 sqrt(dirToPoint.x() * dirToPoint.x() + dirToPoint.y() * dirToPoint.y() + dirToPoint.z() * dirToPoint.z());

    float angle =  acos(scalarP / norm);

    return sin(angle) * Quality::distancePointToPoint(origin, point);
}
