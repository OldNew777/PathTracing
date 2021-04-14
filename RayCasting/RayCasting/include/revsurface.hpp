#ifndef REVSURFACE_HPP
#define REVSURFACE_HPP

#include <tuple>

#include "basicStruct.h"

#include "object3d.hpp"
#include "curve.hpp"
#include "triangle.hpp"
#include "mesh.hpp"
#include "plane.hpp"

#define PI 3.141592653
#define ROTATION_NUM 40
#define DISCRETIZE_NUM 30

#define NEWTON_NUM 10

class RevSurface : public Object3D {
protected:
    Curve *pCurve = nullptr;

    Plane boundingVolumePlane[6];

    //struct Surface {
    //    std::vector<Vector3d> VV;
    //    std::vector<TriangleIndex> VF;
    //} surface;

    //Mesh *meshSurface;

    double  x_min = MAX_DOUBLE, y_min = MAX_DOUBLE, z_min = MAX_DOUBLE,
            x_max = MIN_DOUBLE, y_max = MIN_DOUBLE, z_max = MIN_DOUBLE;
    Vector3d textureColor;


public:
    RevSurface(Curve *pCurve, Material* material) : pCurve(pCurve), Object3D(material) {
        // Check flat.
        for (const auto &cp : pCurve->getControls()) {
            if (cp.z() != 0.0) {
                printf("Profile of revSurface must be flat on xy plane.\n");
                exit(0);
            }
        }

        //// ����1���õ�����ȡ�������ɸ��㣬�γ�mesh
        //std::vector<CurvePoint> curvePoints;
        //pCurve->discretize(DISCRETIZE_NUM, curvePoints);
        //const int steps = ROTATION_NUM;
        //for (unsigned int ci = 0; ci < curvePoints.size(); ++ci) {      // һ��һ����ӽڵ�
        //    const CurvePoint& cp = curvePoints[ci];

        //    for (unsigned int i = 0; i < steps; ++i) {  // �������Ϊ40�ݣ���������ת����
        //        double theta =  2 * PI * (double)i / steps;
        //        Quat4f rot;
        //        rot.setAxisAngle(theta, Vector3d::UP);
        //        
        //        // ��ת������ߵ㣨PA3�еľ��󷨣�z��������-���ţ���Ӱ�첻����Ϊ��Ҫ��תһ��Ȧ
        //        // x���ꡢz������������ν��yһ��Ҫ������
        //        Vector3d pnew = Matrix3f::rotation(rot) * cp.V;                                             // (x*cosa, y, -x*sina)
        //        // Vector3d pnew = Vector3d(cp.V.x() * cos(theta), cp.V.y(), cp.V.x() * sin(theta));        // (x*cosa, y, x*sina)

        //        surface.VV.push_back(pnew);
        //        int i1 = (i + 1 == steps) ? 0 : i + 1;
        //        if (ci != curvePoints.size() - 1) {
        //            surface.VF.push_back(TriangleIndex((ci + 1) * steps + i, ci * steps + i1, ci * steps + i));
        //            surface.VF.push_back(TriangleIndex((ci + 1) * steps + i, (ci + 1) * steps + i1, ci * steps + i1));
        //        }
        //    }
        //}
        //meshSurface = new Mesh(surface.VV, surface.VF, material);

        // ------------------------��Χ��------------------------
        std::vector<CurvePoint> curvePoints;
        pCurve->discretize(DISCRETIZE_NUM, curvePoints);
        for (auto i = curvePoints.begin(); i != curvePoints.end(); ++i) {
            Vector3d point(i->V);
            x_min = std::min(x_min, point.x());
            y_min = std::min(y_min, point.y());

            x_max = std::max(x_max, point.x());
            y_max = std::max(y_max, point.y());
        }

        x_max = max(fabs(x_max), fabs(x_min));
        x_min = -x_max;
        z_max = x_max;
        z_min = x_min;
        boundingVolumePlane[0] = Plane(Vector3d(1, 0, 0), x_min, material);
        boundingVolumePlane[1] = Plane(Vector3d(1, 0, 0), x_max, material);
        boundingVolumePlane[2] = Plane(Vector3d(0, 1, 0), y_min, material);
        boundingVolumePlane[3] = Plane(Vector3d(0, 1, 0), y_max, material);
        boundingVolumePlane[4] = Plane(Vector3d(0, 0, 1), z_min, material);
        boundingVolumePlane[5] = Plane(Vector3d(0, 0, 1), z_max, material);
        // ------------------------��Χ��------------------------
    }

    ~RevSurface() override {
        if (pCurve != nullptr)
            delete pCurve;
        //if (meshSurface != nullptr)
        //    delete meshSurface;
    }

    bool boundingVolumeHit(const Ray& ray, Hit& hit, double tmin)
    {
        //// ����1��������Χ�е�12�����������޽���
        //for (int i = 0; i < 12; ++i) {
        //    if (boundingVolumeTriangle[i].is_intersected(ray, tmin))
        //        return true;
        //}
        //return false;

        // ����2��������Χ�е�6��ƽ�����޽��㣬�����Ƿ��ں���������
        bool result = false;
        for (int i = 0; i < 6; ++i) {
            Hit hit_tmp = hit;
            if (boundingVolumePlane[i].intersect(ray, hit_tmp, tmin)) {
                Vector3d hitPoint = ray.pointAtParameter(hit_tmp.getT());
                if (hitPoint.x() - x_min >= -BOUNDING_EPISION && hitPoint.x() - x_max <= BOUNDING_EPISION &&
                    hitPoint.y() - y_min >= -BOUNDING_EPISION && hitPoint.y() - y_max <= BOUNDING_EPISION &&
                    hitPoint.z() - z_min >= -BOUNDING_EPISION && hitPoint.z() - z_max <= BOUNDING_EPISION) {
                    hit = hit_tmp;
                    result = true;
                }
            }
        }
        return result;
    }

    bool intersect(const Ray & ray, Hit &hit, double tmin) override {
        //// Optional: Change this brute force method into a faster one.
        //bool result = false;
        //for (int triId = 0; triId < (int)surface.VF.size(); ++triId) {
        //    TriangleIndex& triIndex = surface.VF[triId];
        //    Triangle triangle(
        //        surface.VV[surface.VF[triId][0]],
        //        surface.VV[surface.VF[triId][1]],
        //        surface.VV[surface.VF[triId][2]], material);
        //    result |= triangle.intersect(ray, hit, tmin);
        //}
        //return result;

        //// ϸ��Ϊ������Ƭ
        //return meshSurface->intersect(ray, hit, tmin);

        // ����2��������
        // ţ�ٵ���
        Hit hit_bounding;
        if (!boundingVolumeHit(ray, hit_bounding, tmin) || y_max - y_min < epsilon)
            return false;

        Vector3d hitPoint(ray.pointAtParameter(hit_bounding.getT()));

        bool result = false;
        for (int i = 0; i < NEWTON_NUM; ++i) {
            // ţ�ٵ��������� u [0, 1] ��γ�ֵ�������ж�����㡣

            // ��ʼֵȷ������ʱ����
            // t = ���Χ�е�ײ����t
            // u�������
            // theta�������
            Vector3d x, x0, Pt, dPt, F, S;
            double sin_theta, cos_theta, t, u, theta;
            Matrix3d dF;

            x = Vector3d(hit_bounding.getT(), rand0_1(), 2*PI*rand0_1());
            x0 = x;
            t = x0.x(), u = x0.y(), theta = x0.z();

            Pt = pCurve->getPt(u), dPt = pCurve->getdPt(u);
            sin_theta = sin(theta), cos_theta = cos(theta);

            dF = Matrix3d(
                ray.getDirection().x(), -cos_theta * dPt.x(), sin_theta * Pt.x(),
                ray.getDirection().y(), -dPt.y(), 0,
                ray.getDirection().z(), sin_theta * dPt.x(), cos_theta * Pt.x()
            );
            S = Vector3d(Pt.x() * cos_theta, Pt.y(), -Pt.x() * sin_theta);
            F = ray.pointAtParameter(x.x()) - S;

            int times = 40;
            while (F.squaredLength() >= epsilon && times > 0) {        // ��������������times���ҵ����С��epsilonʱ��Ϊ�ҵ��˽�
                --times;
                x = x0 - dF.inverse() * F;

                x0 = x;
                t = x0.x(), u = x0.y(), theta = x0.z();

                Pt = pCurve->getPt(u), dPt = pCurve->getdPt(u);
                sin_theta = sin(theta), cos_theta = cos(theta);

                dF = Matrix3d(
                    ray.getDirection().x(), -cos_theta * dPt.x(),   sin_theta * Pt.x(),
                    ray.getDirection().y(), -dPt.y(),               0,
                    ray.getDirection().z(), sin_theta * dPt.x(),    cos_theta * Pt.x()
                );
                S = Vector3d(Pt.x() * cos_theta, Pt.y(), -Pt.x() * sin_theta);
                F = ray.pointAtParameter(x.x()) - S;
            }

            if (    x.y() >= 0 && x.y() <= 1 &&
                    F.squaredLength() < epsilon && 
                    x.x() > tmin && x.x() < hit.getT()  ) {
                Vector3d dS_du(cos_theta * dPt.x(), dPt.y(), -sin_theta * dPt.x()),
                    dS_dtheta(-sin_theta * Pt.x(), 0, -cos_theta * Pt.x());
                Vector3d normal(Vector3d::cross(dS_du, dS_dtheta));
                if (Vector3d::dot(ray.getDirection(), normal) > 0)
                    normal.negate();

                hit.set(x.x(), material, normal);
                hit.setObject(this);
                result = true;

                if (material->haveTexture())
                    textureColor = material->getTexture_uv(theta / (2 * PI), u);
            }
        }
        return result;
    }

    Vector3d getTextureColor(Vector3d hitPoint) override {
        return textureColor;
    }

    // ��ʱ�ò������麯��override
    Vector3d getNormalOut(Ray ray, Vector3d hitPoint) override { return Vector3d::ZERO; }
};

#endif //REVSURFACE_HPP
