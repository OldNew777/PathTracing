#ifndef CURVE_HPP
#define CURVE_HPP

#include "object3d.hpp"
#include <vecmath.h>
#include <vector>
#include <utility>

#include <algorithm>
#include "basicStruct.h"


// The CurvePoint object stores information about a point on a curve
// after it has been tesselated: the vertex (V) and the tangent (T)
// It is the responsiblility of functions that create these objects to fill in all the data.
struct CurvePoint {
    Vector3d V; // Vertex           顶点
    Vector3d T; // Tangent  (unit)  单位切向量

    CurvePoint(Vector3d V_in, Vector3d T_in) : V(V_in), T(T_in) {}
};




class Curve : public Object3D {
protected:
    std::vector<Vector3d> controls;

public:
    explicit Curve(std::vector<Vector3d> points) : controls(std::move(points)) {}

    std::vector<Vector3d> &getControls() {
        return controls;
    }

    virtual void discretize(int resolution, std::vector<CurvePoint>& data) = 0;
    virtual Vector3d getPt(double t) = 0;
    virtual Vector3d getdPt(double t) = 0;

    // 暂时用不到的虚函数override
    bool intersect(const Ray &r, Hit &h, double tmin) override { return false; }
    Vector3d getNormalOut(Ray ray, Vector3d hitPoint) override { return Vector3d::ZERO; }
    Vector3d getTextureColor(Vector3d hitPoint) override {
        // TODO
        return Vector3d::ZERO;
    }

};




class BezierCurve : public Curve {
public:
    explicit BezierCurve(const std::vector<Vector3d> &points) : Curve(points) {
        if (points.size() < 4 || points.size() % 3 != 1) {
            printf("Number of control points of BezierCurve must be 3n+1!\n");
            exit(0);
        }
    }

    ~BezierCurve() = default;

    void discretize(int resolution, std::vector<CurvePoint>& data) override {
        data.clear();
        // fill in data vector
        for (int j = 0; j < resolution; ++j) {
            double t = j / double(resolution - 1);
            Vector3d Pt = getPt(t), dPt = getdPt(t);
            dPt.normalize();
            data.push_back(CurvePoint(Pt, dPt));
        }
    }

    Vector3d getPt(double t) {
        Vector3d Pt;
        for (int i = 0; i <= n; ++i)
            Pt += controls[i] * Bernstein(i, n, t);
        return Pt;
    }

    Vector3d getdPt(double t) {
        Vector3d dPt;
        for (int i = 0; i <= n - 1; ++i)
            dPt += (controls[i + 1] - controls[i]) * Bernstein(i, n - 1, t);
        return n * dPt;
    }

    inline double Bernstein(int i, int n, double t) {
        if (i < 0 || i > n)
            return 0;
        return comb(n, i) * pow(t, i) * pow(1 - t, n - i);
    }

    long comb(int n, int i) {    // 计算组合数
        if (i <= 0 || n <= 0 || i >= n)
            return 1;
        if (i < n / 2)
            i = n - i;
        long up = 1, down = 1;
        // calculate (i+1)(i+2)...n/(n-i)!
        //for (int j = 1; j <= n - i; ++j) {
        //    int up_multiply = i + j, down_multiply = j;
        //    if (up % down_multiply == 0) {
        //        up /= down_multiply;
        //        down_multiply = 1;
        //    }
        //    if (down % up_multiply == 0) {
        //        down /= up_multiply;
        //        up_multiply = 1;
        //    }
        //    up *= up_multiply;
        //    down *= down_multiply;
        //}
        for (int j = 1; j <= n - i; ++j) {
            up *= i + j;
            down *= j;
        }
        long ans = up / down;
        return ans;
    }

protected:
    int n = controls.size() - 1;
};


#endif // CURVE_HPP
