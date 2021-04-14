#ifndef SPHERE_H
#define SPHERE_H

#include "object3d.hpp"
#include "Vector3d.h"
#include <cmath>
#include <algorithm>

#define PI 3.141592653

class Sphere : public Object3D {
public:
    Sphere() = delete;

    Sphere(const Vector3d &center, double radius, Material *material) : Object3D(material) {
        this->center = center;
        this->radius = radius;
    }

    ~Sphere() override = default;

    bool intersect(const Ray &r, Hit &h, double tmin) override {
        //2nd version, accelerate algorithm
        Vector3d l(center - r.getOrigin());
        double judge_position = radius * radius - l.squaredLength(), t = 1e38,
            tp = Vector3d::dot(l, r.getDirection().normalized());
        
        if (judge_position > 0) //light in the sphere, there must be intersection
        {
            double t0_squared = judge_position + tp * tp;
            t = tp + std::sqrt(t0_squared);

            if (t < tmin || t >= h.getT())
                return false;
            h.set(t, material, (center - r.pointAtParameter(t)).normalized());
        }
        else if (judge_position < 0)    //light outside the sphere
        {
            //no intersection because the ray shoot opposite the sphere
            if (tp < 0)
                return false;

            double t0_squared = judge_position + tp * tp;
            //no intersection because it's too far
            if (t0_squared < 0)
                return false;

            t = tp - std::sqrt(t0_squared);
            if (t < tmin || t >= h.getT())
                return false;
            h.set(t, material, (r.pointAtParameter(t) - center).normalized());
        }
        else    //judge_position == 0, light right on the surface of the sphere
        {
            if (tp <= 0)
                return false;
            else {
                if (0 < tmin || 0 >= h.getT())
                    return false;
                h.set(0, material, (r.getOrigin() - center).normalized());
            }
        }

        h.setObject(this);
        return true;
    }

    Vector3d getNormalOut(Ray ray, Vector3d hitPoint) override {
        return (hitPoint - center).normalized();
    }

    Vector3d getTextureColor(Vector3d hitPoint) override {
        Vector3d direction = (hitPoint - center).normalized();
        double  phi = atan2(direction.z(), direction.x()),
                theta = asin(direction.y());
        double  u = 0.5 - phi / (2 * PI),
                v = 0.5 + theta / PI;
        return material->getTexture_uv(u, v);
    }

protected:
    Vector3d center;
    double radius;

};


#endif
