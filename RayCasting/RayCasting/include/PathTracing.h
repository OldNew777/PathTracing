#pragma once

#include "ray.hpp"
#include "Group.hpp"
#include "camera.hpp"
#include "function.h"

//Ray rayGenerator(Ray &cam, double x, double y);
Vector3d computePathTracing(Group* baseGroup, Ray ray, Vector3d backGroundColor, int depths = 0);
