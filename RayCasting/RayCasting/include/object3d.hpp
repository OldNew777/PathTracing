#ifndef OBJECT3D_H
#define OBJECT3D_H

#include "ray.hpp"
#include "hit.hpp"
#include "material.hpp"

// Base class for all 3d entities.
class Object3D {
public:
    Object3D() : material(nullptr) {}

    virtual ~Object3D() = default;

    explicit Object3D(Material *material) {
        this->material = material;
    }

    // 永远朝外的normal
    virtual Vector3d getNormalOut(Ray ray, Vector3d hitPoint) = 0;

    // Intersect Ray with this object. If hit, store information in hit structure.
    virtual bool intersect(const Ray &r, Hit &h, double tmin) = 0;

    // 获取对应点的贴图颜色
    virtual Vector3d getTextureColor(Vector3d hitPoint) = 0;
protected:

    Material *material;
};

#endif

