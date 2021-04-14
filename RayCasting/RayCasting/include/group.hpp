#ifndef GROUP_H
#define GROUP_H


#include "object3d.hpp"
#include "ray.hpp"
#include "hit.hpp"
#include <iostream>
#include <vector>


class Group : public Object3D {

public:

    Group() = delete;

    explicit Group (int num_objects = 0) : size(num_objects){
        objects = new std::vector<Object3D*>(size);
    }

    ~Group() override {
        for (auto i = objects->begin(); i != objects->end(); ++i)
            delete (*i);
        delete objects;
    }

    bool intersect(const Ray &ray, Hit &hit, double tmin) override {
        bool intersected = false;
        for (auto i = objects->begin(); i != objects->end(); ++i){
            if ((*i)->intersect(ray, hit, tmin))
                intersected = true;
        }
        return intersected;
    }

    void addObject(int index, Object3D *obj) {
        (*objects)[index] = obj;
    }

    int getGroupSize() const{
        return size;
    }

    Vector3d getNormalOut(Ray ray, Vector3d hitPoint) override {
        // TODO
        return Vector3d::ZERO;
    }

    Vector3d getTextureColor(Vector3d hitPoint) override {
        // TODO
        return Vector3d::ZERO;
    }

private:
    int size;
    std::vector<Object3D*>* objects;
};

#endif
	
