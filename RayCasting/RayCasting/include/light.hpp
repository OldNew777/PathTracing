#ifndef LIGHT_H
#define LIGHT_H

#include <Vector3d.h>
#include "object3d.hpp"

class Light {
public:
    Light() = default;

    virtual ~Light() = default;

    virtual void getIllumination(const Vector3d &p, Vector3d &dir, Vector3d &col) const = 0;
};


class DirectionalLight : public Light {
public:
    DirectionalLight() = delete;

    DirectionalLight(const Vector3d &d, const Vector3d &c) {
        direction = d.normalized();
        color = c;
    }

    ~DirectionalLight() override = default;

    ///@param p unsed in this function
    ///@param distanceToLight not well defined because it's not a point light
    void getIllumination(const Vector3d &p, Vector3d &dir, Vector3d &col) const override {
        // the direction to the light is the opposite of the
        // direction of the directional light source
        dir = -direction;
        col = color;
    }

private:

    Vector3d direction;
    Vector3d color;

};

class PointLight : public Light {
public:
    PointLight() = delete;

    PointLight(const Vector3d &p, const Vector3d &c) {
        position = p;
        color = c;
    }

    ~PointLight() override = default;

    void getIllumination(const Vector3d &p, Vector3d &dir, Vector3d &col) const override {
        // the direction to the light is the opposite of the
        // direction of the directional light source
        dir = (position - p);
        dir = dir / dir.length();
        col = color;
    }

private:

    Vector3d position;
    Vector3d color;

};

#endif // LIGHT_H
