#ifndef MESH_H
#define MESH_H

#include <vector>
#include "Vector2d.h"
#include "Vector3d.h"
#include "object3d.hpp"
#include "triangle.hpp"
#include "plane.hpp"
#include "basicStruct.h"

#define BOUNDING_VOLUME_SIZE 30
#define BOUNDING_EPISION 1e-4
#define MIN_DOUBLE -1e250
#define MAX_DOUBLE 1e250


class Mesh : public Object3D {

public:
    Mesh(const char *filename, Material *m);
    Mesh(const std::vector<Vector3d>& v, const std::vector<TriangleIndex>& t, Material* m);
    
    ~Mesh();

    void separate(Vector3d average);
    void addTriangle(
        vector<TriangleIndex> &triangleIndex_children,
        vector<Vector3d> &vertex_children,
        vector<int> &pointPtr,
        TriangleIndex& triangle);
    void processBounding(Vector3d& average);

    bool intersect(const Ray &ray, Hit &hit, double tmin) override;
    
    Vector3d getNormalOut(Ray ray, Vector3d hitPoint) override;

    Vector3d getTextureColor(Vector3d hitPoint) override;

protected:
    bool boundingVolumeHit(const Ray& ray, double tmin);

    std::vector<Vector3d> vertex;
    std::vector<TriangleIndex> triangleIndex;

    double  x_min, y_min, z_min,
            x_max, y_max, z_max;
    //Triangle boundingVolumeTriangle[12];
    Plane boundingVolumePlane[6];       // 对整个mesh的group做一个包围盒，然后依次在内部建立空间八叉树

    Mesh* children[8];
};

#endif
