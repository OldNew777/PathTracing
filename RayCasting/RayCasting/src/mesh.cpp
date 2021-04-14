#include "mesh.hpp"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <utility>
#include <sstream>
#include <string.h>

using namespace std;

bool Mesh::intersect(const Ray &ray, Hit &hit, double tmin) {
    if (!boundingVolumeHit(ray, tmin))
        return false;

    bool result = false;
    if (children[0] != nullptr) {   // 仍然未到叶子结点
        for (int i = 0; i < 8; ++i)
            result |= children[i]->intersect(ray, hit, tmin);
    }
    else {          // 到了叶子结点，直接求与众triangle的交点
        for (int triId = 0; triId < (int)triangleIndex.size(); ++triId) {
            TriangleIndex& triIndex = triangleIndex[triId];
            Triangle triangle(
                vertex[triIndex[0]], vertex[triIndex[1]], 
                vertex[triIndex[2]], material
            );
            result |= triangle.intersect(ray, hit, tmin);
        }
    }
    return result;
}

Vector3d Mesh::getNormalOut(Ray ray, Vector3d hitPoint)
{
    // TODO
    return Vector3d::ZERO;
}

Mesh::Mesh(const char *filename, Material *material) : Object3D(material) {

    // Optional: Use tiny obj loader to replace this simple one.
    std::ifstream f;
    f.open(filename);
    if (!f.is_open()) {
        std::cout << "Cannot open " << filename << "\n";
        return;
    }
    std::string line;
    std::string vTok("v");
    std::string fTok("f");
    std::string texTok("vt");
    char bslash = '/', space = ' ';
    std::string tok;
    int texID;
    while (true) {
        std::getline(f, line);
        if (f.eof()) {
            break;
        }
        if (line.size() < 3) {
            continue;
        }
        if (line.at(0) == '#') {
            continue;
        }
        std::stringstream ss(line);
        ss >> tok;
        if (tok == vTok) {              // 顶点vertex（vector3d）
            Vector3d vec;
            ss >> vec[0] >> vec[1] >> vec[2];
            vertex.push_back(vec);
        } else if (tok == fTok) {       // 面face（三角形）
            if (line.find(bslash) != std::string::npos) {
                std::replace(line.begin(), line.end(), bslash, space);
                std::stringstream facess(line);
                TriangleIndex trig;
                facess >> tok;
                for (int ii = 0; ii < 3; ii++) {
                    facess >> trig[ii] >> texID;
                    trig[ii]--;
                }
                triangleIndex.push_back(trig);
            } else {
                TriangleIndex trig;
                for (int ii = 0; ii < 3; ii++) {
                    ss >> trig[ii];
                    trig[ii]--;
                }
                triangleIndex.push_back(trig);
            }
        } else if (tok == texTok) {
            Vector2d texcoord;
            ss >> texcoord[0];
            ss >> texcoord[1];
        }
    }

    f.close();

    for (int i = 0; i < 8; ++i)
        children[i] = nullptr;

    Vector3d average;
    processBounding(average);

    if (vertex.size() <= BOUNDING_VOLUME_SIZE)    // 包围的空间内mesh数量很少，停止八分
        return;

    separate(average);
}

Mesh::Mesh(const std::vector<Vector3d>& v, const std::vector<TriangleIndex>& t, Material* m) :
    Object3D(m), vertex(v), triangleIndex(t)
{
    for (int i = 0; i < 8; ++i)
        children[i] = nullptr;

    Vector3d average;
    processBounding(average);

    if (v.size() <= BOUNDING_VOLUME_SIZE)         // 包围的空间内mesh数量很少，停止八分
        return;

    separate(average);
}

void Mesh::separate(Vector3d average)
{
    vector<TriangleIndex> triangleIndex_children[8];
    vector<Vector3d> vertex_children[8];
    vector<int> pointPtr[8];

    for (int i = 0; i < 8; ++i) {
        pointPtr[i].resize(vertex.size());
        for (auto j = 0; j < pointPtr[i].size(); ++j)
            pointPtr[i][j] = -1;
    }

    bool added[8];

    for (auto i = triangleIndex.begin(); i != triangleIndex.end(); ++i) {
        TriangleIndex& triangle = (*i);
        for (int j = 0; j < 8; ++j)
            added[j] = false;

        for (int j = 0; j < 3; ++j) {
            Vector3d point = vertex[triangle[j]];
            if (point.x() <= average.x()) {
                if (point.y() <= average.y()) {
                    if (point.z() <= average.z() && !added[0]) {
                        addTriangle(
                            triangleIndex_children[0],
                            vertex_children[0],
                            pointPtr[0],
                            triangle
                        );
                        added[0] = true;
                    }
                    if (point.z() >= average.z() && !added[1]) {
                        addTriangle(
                            triangleIndex_children[1],
                            vertex_children[1],
                            pointPtr[1],
                            triangle
                        );
                        added[1] = true;
                    }
                }
                if (point.y() >= average.y()) {
                    if (point.z() <= average.z() && !added[2]) {
                        addTriangle(
                            triangleIndex_children[2],
                            vertex_children[2],
                            pointPtr[2],
                            triangle
                        );
                        added[2] = true;
                    }
                    if (point.z() >= average.z() && !added[3]) {
                        addTriangle(
                            triangleIndex_children[3],
                            vertex_children[3],
                            pointPtr[3],
                            triangle
                        );
                        added[3] = true;
                    }
                }
            }
            if (point.x() >= average.x()) {
                if (point.y() <= average.y()) {
                    if (point.z() <= average.z() && !added[4]) {
                        addTriangle(
                            triangleIndex_children[4],
                            vertex_children[4],
                            pointPtr[4],
                            triangle
                        );
                        added[4] = true;
                    }
                    if (point.z() >= average.z() && !added[5]) {
                        addTriangle(
                            triangleIndex_children[5],
                            vertex_children[5],
                            pointPtr[5],
                            triangle
                        );
                        added[5] = true;
                    }
                }
                if (point.y() >= average.y()) {
                    if (point.z() <= average.z() && !added[6]) {
                        addTriangle(
                            triangleIndex_children[6],
                            vertex_children[6],
                            pointPtr[6],
                            triangle
                        );
                        added[6] = true;
                    }
                    if (point.z() >= average.z() && !added[7]) {
                        addTriangle(
                            triangleIndex_children[7],
                            vertex_children[7],
                            pointPtr[7],
                            triangle
                        );
                        added[7] = true;
                    }
                }
            }
        }
    }

    for (int i = 0; i < 8; ++i)
        if (triangleIndex.size() - triangleIndex_children[i].size() < (double(BOUNDING_VOLUME_SIZE) / 4))
            // 若再进行细分，无法有效降低分支中的三角形个数则停止
            // 也起到一个防止死循环的作用
            return;

    for (int i = 0; i < 8; ++i)
        children[i] = new Mesh(vertex_children[i], triangleIndex_children[i], material);

    // 清空当前结点的mesh信息
    vector<Vector3d>().swap(vertex);
    vector<TriangleIndex>().swap(triangleIndex);
}

void Mesh::addTriangle(
    vector<TriangleIndex>& triangleIndex_children, 
    vector<Vector3d>& vertex_children, 
    vector<int>& pointPtr, 
    TriangleIndex& triangle)
{
    //// 暴力硬塞，三角形顶点不复用
    //for (int i = 0; i < 3; ++i)
    //    vertex_children.push_back(vertex[triangle[i]]);
    //triangleIndex_children.push_back(
    //    TriangleIndex(
    //        vertex_children.size() - 3,
    //        vertex_children.size() - 2,
    //        vertex_children.size() - 1)
    //);

    // 三角形顶点复用
    for (int i = 0; i < 3; ++i)
        if (pointPtr[triangle[i]] < 0) {
            pointPtr[triangle[i]] = vertex_children.size();
            vertex_children.push_back(vertex[triangle[i]]);
        }
    triangleIndex_children.push_back(
        TriangleIndex(
            pointPtr[triangle[0]],
            pointPtr[triangle[1]],
            pointPtr[triangle[2]])
    );
}

void Mesh::processBounding(Vector3d& average)
{
    x_min = MAX_DOUBLE, y_min = MAX_DOUBLE, z_min = MAX_DOUBLE;
    x_max = MIN_DOUBLE, y_max = MIN_DOUBLE, z_max = MIN_DOUBLE;
    average = Vector3d::ZERO;
    for (auto i = vertex.begin(); i != vertex.end(); ++i) {
        x_min = std::min(x_min, i->x());
        y_min = std::min(y_min, i->y());
        z_min = std::min(z_min, i->z());

        x_max = std::max(x_max, i->x());
        y_max = std::max(y_max, i->y());
        z_max = std::max(z_max, i->z());

        average += (*i);
    }
    average = average / vertex.size();

    //// 方法1，将包围盒分为12个三角形与光线求交
    //Vector3d boundingVolumePoints[8];
    //boundingVolumePoints[0] = Vector3d(x_min, y_max, z_min);
    //boundingVolumePoints[1] = Vector3d(x_max, y_max, z_min);
    //boundingVolumePoints[2] = Vector3d(x_max, y_max, z_max);
    //boundingVolumePoints[3] = Vector3d(x_min, y_max, z_max);
    //boundingVolumePoints[4] = Vector3d(x_min, y_min, z_min);
    //boundingVolumePoints[5] = Vector3d(x_max, y_min, z_min);
    //boundingVolumePoints[6] = Vector3d(x_max, y_min, z_max);
    //boundingVolumePoints[7] = Vector3d(x_min, y_min, z_max);

    //boundingVolumeTriangle[0] = Triangle(boundingVolumePoints[0], boundingVolumePoints[1], boundingVolumePoints[3], material);
    //boundingVolumeTriangle[1] = Triangle(boundingVolumePoints[1], boundingVolumePoints[2], boundingVolumePoints[3], material);
    //boundingVolumeTriangle[2] = Triangle(boundingVolumePoints[0], boundingVolumePoints[3], boundingVolumePoints[7], material);
    //boundingVolumeTriangle[3] = Triangle(boundingVolumePoints[0], boundingVolumePoints[4], boundingVolumePoints[7], material);
    //boundingVolumeTriangle[4] = Triangle(boundingVolumePoints[0], boundingVolumePoints[1], boundingVolumePoints[4], material);
    //boundingVolumeTriangle[5] = Triangle(boundingVolumePoints[1], boundingVolumePoints[4], boundingVolumePoints[5], material);
    //boundingVolumeTriangle[6] = Triangle(boundingVolumePoints[1], boundingVolumePoints[2], boundingVolumePoints[6], material);
    //boundingVolumeTriangle[7] = Triangle(boundingVolumePoints[1], boundingVolumePoints[5], boundingVolumePoints[6], material);
    //boundingVolumeTriangle[8] = Triangle(boundingVolumePoints[4], boundingVolumePoints[5], boundingVolumePoints[7], material);
    //boundingVolumeTriangle[9] = Triangle(boundingVolumePoints[5], boundingVolumePoints[6], boundingVolumePoints[7], material);
    //boundingVolumeTriangle[10] = Triangle(boundingVolumePoints[2], boundingVolumePoints[3], boundingVolumePoints[7], material);
    //boundingVolumeTriangle[11] = Triangle(boundingVolumePoints[2], boundingVolumePoints[6], boundingVolumePoints[7], material);

    // 方法2，将包围盒分为6个平面与光线求交
    boundingVolumePlane[0] = Plane(Vector3d(1, 0, 0), x_min, material);
    boundingVolumePlane[1] = Plane(Vector3d(1, 0, 0), x_max, material);
    boundingVolumePlane[2] = Plane(Vector3d(0, 1, 0), y_min, material);
    boundingVolumePlane[3] = Plane(Vector3d(0, 1, 0), y_max, material);
    boundingVolumePlane[4] = Plane(Vector3d(0, 0, 1), z_min, material);
    boundingVolumePlane[5] = Plane(Vector3d(0, 0, 1), z_max, material);
}

Mesh::~Mesh()
{
    for (int i = 0; i < 8; ++i)
        if (children[i] != nullptr)
            delete children[i];
}

Vector3d Mesh::getTextureColor(Vector3d hitPoint) {
    // TODO
    return Vector3d::ZERO;
}

bool Mesh::boundingVolumeHit(const Ray& ray, double tmin)
{
    //// 方法1，求解与包围盒的12个三角形有无交点
    //for (int i = 0; i < 12; ++i) {
    //    if (boundingVolumeTriangle[i].is_intersected(ray, tmin))
    //        return true;
    //}
    //return false;

    // 方法2，求解与包围盒的6个平面有无交点，交点是否在合理区域内
    for (int i = 0; i < 6; ++i) {
        Hit hit;
        if (boundingVolumePlane[i].intersect(ray, hit, tmin)) {
            Vector3d hitPoint = ray.pointAtParameter(hit.getT());
            if (hitPoint.x() - x_min >= -BOUNDING_EPISION && hitPoint.x() - x_max <= BOUNDING_EPISION &&
                hitPoint.y() - y_min >= -BOUNDING_EPISION && hitPoint.y() - y_max <= BOUNDING_EPISION &&
                hitPoint.z() - z_min >= -BOUNDING_EPISION && hitPoint.z() - z_max <= BOUNDING_EPISION)
                return true;
        }
    }
    return false;
}
