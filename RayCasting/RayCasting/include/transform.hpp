#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <vecmath.h>
#include "object3d.hpp"

// transforms a 3D point using a matrix, returning a 3D point
static Vector3d transformPoint(const Matrix4d &mat, const Vector3d &point) {
    return (mat * Vector4d(point, 1)).xyz();
}

// transform a 3D directino using a matrix, returning a direction
static Vector3d transformDirection(const Matrix4d &mat, const Vector3d &dir) {
    return (mat * Vector4d(dir, 0)).xyz();
}

// TODO: implement this class so that the intersect function first transforms the ray
class Transform : public Object3D {
public:
    Transform() {}

    Transform(const Matrix4d &m, Object3D *obj) : object(obj) {
        transform = m.inverse();
    }

    ~Transform() {
    }

    bool intersect(const Ray &r, Hit &h, double tmin) override {
        // ����PA�б任direction�㷨�ľ��ȹ��ͣ��ᵼ���ཻ�㷨��bug��������Ƭ��ʧ���Ŵ��ʴﵽ20ʱ����Ƭȫ����ʧ
        // �����������޸ģ�ȡԭ����10000�����ϵĵ㣬��originһ������任���˺��ټ���任��ķ���
        // �����޸ģ�transform�ľ��ȴ�����
        Vector3d trSource = transformPoint(transform, r.getOrigin()), trTarget = transformPoint(transform, r.pointAtParameter(10000));
        Vector3d trDirection = (trTarget - trSource).normalized();
        Ray tr(trSource, trDirection);
        bool inter = object->intersect(tr, h, tmin);
        if (inter) {
            h.set(h.getT(), h.getMaterial(), transformDirection(transform.transposed(), h.getNormal()).normalized());
        }
        return inter;
    }

    Vector3d getNormalOut(Ray ray, Vector3d hitPoint) override {
        return object->getNormalOut(ray, hitPoint);
    }

    Vector3d getTextureColor(Vector3d hitPoint) override {
        return object->getTextureColor(hitPoint);
    }

protected:
    Object3D *object; //un-transformed object
    Matrix4d transform;
};

#endif //TRANSFORM_H
