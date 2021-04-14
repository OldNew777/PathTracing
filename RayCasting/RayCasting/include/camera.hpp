#ifndef CAMERA_H
#define CAMERA_H

#include <vecmath.h>
#include <cmath>
#include <iostream>

#include "function.h"
#include "ray.hpp"

#define epsilon 1e-4
#define PI 3.141592653

enum class Position
{
    UP = 1,
    DOWN = 2,
    LEFT = 3,
    RIGHT = 4
};

class Camera {
public:
    Camera(const Vector3d &origin, const Vector3d &direction, const Vector3d &up, int imgW, int imgH, double canvasDistance, double focusDistance, double interfereRatio) :
        origin(origin), width(imgW), height(imgH), canvasDistance(canvasDistance), focusDistance(focusDistance), interfereRatio(interfereRatio){
        this->direction = direction.normalized();
        this->horizontal = Vector3d::cross(this->direction, up);
        this->up = Vector3d::cross(this->horizontal, this->direction);

        if (interfereRatio < epsilon)
            fieldDepth = false;
        else
            fieldDepth = true;
    }

    virtual ~Camera() = default;

    // Generate rays for each screen-space coordinate
    virtual Ray generateRay(const Vector2d &point, const Vector2d &s) = 0;
    virtual Vector3d getInterfere() = 0;

    int getWidth() const { return width; }
    int getHeight() const { return height; }

protected:
    // Extrinsic parameters
    Vector3d origin;
    Vector3d direction;
    Vector3d up;
    Vector3d horizontal;
    // Intrinsic parameters
    int width;
    int height;

    // focus parameters
    double focusDistance;
    double canvasDistance = 140.;
    double interfereRatio;
    bool fieldDepth;

    // transform parameters
    Vector3d cx, cy;
};

class PerspectiveCamera : public Camera {

public:
    PerspectiveCamera(const Vector3d &origin, const Vector3d &direction,
            const Vector3d &up, int imgW, int imgH, double canvasDistance, double focusDistance, double ratio_interfere) :
        Camera(origin, direction, up, imgW, imgH, canvasDistance, focusDistance, ratio_interfere) {

        cx = Vector3d(width * .5135 / height, 0, 0);
        cy = (Vector3d::cross(cx, direction)).normalized() * .5135;

        // unit vector in plane : ax + by + cz = d
    }

    virtual Vector3d getInterfere() {
        //// 方法1，单位球内压缩至与焦平面平行的平面
        //// 缺点：横向的矢量会增多，导致出现横向散光
        //double x = 0, y = 0, z = 0;
        //double theta = rand0_1() * PI, phi = 2 * rand0_1() * PI;
        //x = sin(theta) * cos(phi);
        //y = sin(theta) * sin(phi);
        //z = cos(theta);
        //x = rand0_x(x);
        //y = rand0_x(y);
        //z = rand0_x(z);

        //Vector3d interfere(x, y, z);
        //interfere -= direction * Vector3d::dot(interfere, direction);

        // 方法2，直接在焦平面内部求一个单位圆
        double theta = rand0_1() * 2 * PI;
        double up_para = rand0_x_concentrated(sin(theta)), horizontal_para = rand0_x_concentrated(cos(theta));
        if (rand() % 2 == 0)
            up_para = -up_para;
        if (rand() % 2 == 0)
            horizontal_para = -horizontal_para;

        Vector3d interfere = up_para * up + horizontal_para * horizontal;

        return interfere;
    }

    virtual Ray generateRay(const Vector2d &point, const Vector2d &interfere_AntiAliasing) {
        double r1 = 2 * rand0_1(), dx = r1 < 1 ? sqrt(r1) - 1 : 1 - sqrt(2 - r1);
        double r2 = 2 * rand0_1(), dy = r2 < 1 ? sqrt(r2) - 1 : 1 - sqrt(2 - r2);

        // rayDirection
        // 相当于transform前，z轴前进1单位，xy坐标的变化值
        Vector3d rayDirection = cx * (((interfere_AntiAliasing.x() + .5 + dx) / 2 + point.x()) / width - .5) +
            cy * (((interfere_AntiAliasing.y() + .5 + dy) / 2 + point.y()) / height - .5) + direction;

        Vector3d camRayOrigin(origin + rayDirection * canvasDistance);

        if (fieldDepth) {
            // 从画面上ray发生点到焦平面上对应点的向量
            Vector3d ray = rayDirection * (focusDistance - canvasDistance);
            // 焦平面上对应点
            Vector3d focusPoint = camRayOrigin + ray;
            
            // 扰动画面上的ray发生点
            Vector3d origin_interfere(getInterfere());
            
            camRayOrigin = interfereRatio * origin_interfere + origin;

            rayDirection = (focusPoint - camRayOrigin).normalized();
            camRayOrigin += rayDirection * canvasDistance / Vector3d::dot(rayDirection, direction);
        }

        return Ray(camRayOrigin, rayDirection.normalized());
    }
};

#endif //CAMERA_H
