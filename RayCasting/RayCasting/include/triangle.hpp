#ifndef TRIANGLE_H
#define TRIANGLE_H

#define epsilon 1e-5

#include "object3d.hpp"
#include "vecmath.h"
#include <cmath>
#include <iostream>

using namespace std;

class mesh;

class Triangle : public Object3D {

public:
	friend class mesh;
	Triangle() {}

	// a b c are three vertex positions of the triangle
	Triangle(const Vector3d& a, const Vector3d& b, const Vector3d& c, Material* m) : Object3D(m) {
		vertices[0] = a;
		vertices[1] = b;
		vertices[2] = c;

		//calculate normal direction
		normal = Vector3d::cross(a - b, a - c).normalized();
	}

	Triangle(const Triangle& t) {
		vertices[0] = t.vertices[0];
		vertices[1] = t.vertices[1];
		vertices[2] = t.vertices[2];
		normal = t.normal;
	}

	bool intersect(const Ray& ray, Hit& hit, double tmin) override {
		//use the way in PPT, calculate directly
		Vector3d Rd(ray.getDirection()),
			E1(vertices[0] - vertices[1]),
			E2(vertices[0] - vertices[2]);
		double bottom = Matrix3d(Rd, E1, E2).determinant();
		//no intersection or parallel with the plane
		if (fabs(bottom) < epsilon)
			return false;
		Vector3d S(vertices[0] - ray.getOrigin());
		double t = Matrix3d(S, E1, E2).determinant() / bottom,
			beta = Matrix3d(Rd, S, E2).determinant() / bottom,
			gama = Matrix3d(Rd, E1, S).determinant() / bottom;

		if (t < tmin || t >= hit.getT() ||
			beta < 0 || gama < 0 || (beta + gama > 1))
			return false;

		Vector3d norm(normal);
		if (Vector3d::dot(ray.getDirection(), norm) > 0)
			norm.negate();
		hit.set(t, material, norm);
		hit.setObject(this);
		return true;
	}

	Vector3d getNormalOut(Ray ray, Vector3d hitPoint) override {
		// TODO
		return Vector3d::ZERO;
	}

	Vector3d getTextureColor(Vector3d hitPoint) override {
		// TODO
		return Vector3d::ZERO;
	}

protected:

	Vector3d normal;
	Vector3d vertices[3];
};

#endif //TRIANGLE_H
