#ifndef HIT_H
#define HIT_H

#include <vecmath.h>
#include "ray.hpp"

class Material;
class Object3D;

class Hit {
public:

    // constructors
    Hit() {
        material = nullptr;
        object = nullptr;
        t = 1e38;
    }

    Hit(double _t, Material *m, const Vector3d &n) {
        t = _t;
        material = m;
        normal = n;
        object = nullptr;
    }

    Hit(const Hit &h) {
        t = h.t;
        material = h.material;
        normal = h.normal;
        object = h.object;
    }

    // destructor
    ~Hit() = default;

    double getT() const {
        return t;
    }

    Material *getMaterial() const {
        return material;
    }

    const Vector3d&getNormal() const {
        return normal;
    }

    Object3D* getObject() const {
        return object;
    }

    void set(double _t, Material *m, const Vector3d&n) {
        t = _t;
        material = m;
        normal = n;
    }

    void setObject(Object3D* object) {
        this->object = object;
    }

private:
    double t;
    Material *material;
    Vector3d normal;
    Object3D *object;
};

inline std::ostream &operator<<(std::ostream &os, const Hit &h) {
    os << "Hit <" << h.getT() << ", " << h.getNormal() << ">";
    return os;
}

#endif // HIT_H
