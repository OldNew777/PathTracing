#ifndef PLANE_H
#define PLANE_H

#define epsilon 1e-4

#include <cmath>

#include "object3d.hpp"
#include "Vector3d.h"
#include "function.h"

// function: ax+by+cz=d

class Plane : public Object3D {
public:
    Plane() {}

    Plane(const Vector3d &normal, double d, Material *m) : Object3D(m){
        this->normal = normal;
        this->d = d;

        Vector3d randVec;
        while (i_unit == Vector3d::ZERO) {
            randVec = Vector3d(rand0_1(), rand0_1(), rand0_1());
            i_unit = randVec - Vector3d::dot(randVec, normal) / normal.length();
        }
        i_unit.normalize();
        j_unit = Vector3d::cross(normal, i_unit).normalized();

        // 随意在平面上取一点作为原点，暂时未定位固定点贴图，选择的贴图也是无序图
        // 所以贴图的内容暂时具有一定的随机性
        double x0 = rand() % 10, y0 = rand() % 10, z0 = rand() % 10;
        if (fabs(normal.z()) < epsilon) {   // c == 0
            if (fabs(normal.y()) < epsilon) // b == 0
                x0 = d / normal.x();
            else                            // b != 0
                y0 = (d - normal.x() * x0) / normal.y();
        }
        else                                // c != 0
            z0 = (d - normal.x() * x0 - normal.y() * y0) / normal.z();
        oPoint = Vector3d(x0, y0, z0);
    }

    ~Plane() override = default;

    bool intersect(const Ray &r, Hit &h, double tmin) override {
        Vector3d ray_direction = r.getDirection(), ray_origin = r.getOrigin();
        double bottom = Vector3d::dot(normal, ray_direction);

        //if bottom is too small and close to 0
        //we think them parallel
        if (fabs(bottom) < epsilon)
            return false;

        double top = d - Vector3d::dot(normal, ray_origin);
        double t = top / bottom;

        //time limit(including t < 0), give updating
        if (t < tmin || t >= h.getT())
            return false;

        //intersection
        Vector3d norm(normal);
        if (Vector3d::dot(r.getDirection(), norm) > 0)
            norm.negate();
        h.set(t, material, norm);
        h.setObject(this);
        return true;
    }

    Vector3d getNormalOut(Ray ray, Vector3d hitPoint) override {
        // TODO
        return Vector3d::ZERO;
    }

    Vector3d getTextureColor(Vector3d hitPoint) override {
        double x, y;
        Vector3d coordinate = hitPoint - oPoint;
        x = Vector3d::dot(coordinate, i_unit);
        y = Vector3d::dot(coordinate, j_unit);
        return material->getTexture_xy(x, y, 0.1);
    }

protected:
    Vector3d normal;
    double d;

    // 基向，以及原点
    Vector3d i_unit, j_unit, oPoint;
};

#endif //PLANE_H
