#include <algorithm>
#include <random>

#include "PathTracing.h"
#include "hit.hpp"
#include "light.hpp"
#include "ray.hpp"

#define M_PI 3.141592653
#define TRACE_DEPTH 5
#define epsilon 1e-4


double  nc = 1, nt = 1.5;   // �����벣����������

using namespace std;


Vector3d computePathTracing(Group* baseGroup, Ray ray,Vector3d backGroundColor, int depths)
{
    Hit hit;
    Object3D* object = nullptr;
    bool isIntersect = baseGroup->intersect(ray, hit, epsilon);   // �ҳ����������ĵ���������
    if (!isIntersect) {
        if (depths == 0)
            return backGroundColor;
        else
            return Vector3d::ZERO;
    }

    object = hit.getObject();
    // ���㷨��
    Vector3d normal(hit.getNormal()), hitPoint(ray.pointAtParameter(hit.getT()));
    // ���㾵�淴�䷽��
    Vector3d reflectDirection(ray.getDirection() - 2 * normal * Vector3d::dot(normal, ray.getDirection()));
    reflectDirection.normalize();

    Material* material = hit.getMaterial();
    Vector3d objectColor, emssionColor(material->getEmissionColor());
    double refractRate(material->getRefractRate()), specularRate(material->getSpecularRate());

    // �������ͼ����ȡ������ɫ���������ͼ����ȡhitPoint��Ӧ��ͼ�ϵ���ɫ
    if (material->haveTexture()) {
        objectColor = object->getTextureColor(hitPoint);
    } 
    else {
        objectColor = material->getColor();
    }

    Vector3d finalColor(emssionColor);    // Ĭ��Ϊ���巢��ɫ

    double color_maxOfRGB = objectColor.max();

    //�ݹ����̫�󣬻����������ɫ�����Ѿ��Ǻ�ɫ
    if (++depths > TRACE_DEPTH || objectColor == Vector3d::ZERO) {
        /*if (rand0_1() < color_maxOfRGB)
            objectColor = objectColor / color_maxOfRGB;
        else*/
            return emssionColor;
    }

    // �����䣨DIFF��
    // ��������������䣬��ô���������һ��������������䡣
    if (1 - refractRate - specularRate > epsilon) {
        double alpha = 2 * M_PI * rand0_1(), r2 = rand0_1(), r2s = sqrt(r2);
        Vector3d u = (Vector3d::cross((fabs(normal.x()) > .1 ? Vector3d(0, 1, 0) : Vector3d(1, 0, 0)), normal)).normalized();
        Vector3d v = Vector3d::cross(normal, u);  //normal��v��uΪ������
        Vector3d d = (u * cos(alpha) * r2s + v * sin(alpha) * r2s + normal * sqrt(1 - r2)).normalized();
        finalColor += (1 - refractRate - specularRate) * objectColor * computePathTracing(baseGroup, Ray(hitPoint, d), backGroundColor, depths);
    }
    
    // ���淴�䣨����ΪSPEC�� �������ȫ���淴��
    // ���淴�䣬���㾵�淴��ķ���Ȼ������ݹ�
    if (specularRate > 0 && refractRate == 0)
        finalColor += specularRate * objectColor *
            computePathTracing(baseGroup, Ray(hitPoint, reflectDirection), backGroundColor, depths);

    // ��������䣨����ΪREFR�� 
    // �������ʣ���һ���ֹ���з��䣬��һ���ֹ�������䡣 
    // ��ʵ���������
    else if (refractRate > 0) {
        Ray reflectRay(hitPoint, reflectDirection);
        Vector3d N;
        N = object->getNormalOut(ray, hitPoint);
        // �����Ƿ�ӿ������벣��
        bool into = Vector3d::dot(N, normal) > epsilon;

        double  nnt = into ? nc / nt : nt / nc,     // sin(a2) / sin(a1)
                ddn = Vector3d::dot(normal, ray.getDirection()), cos2t;
        
        // cos(a1) = -ddn
        // 1- ddn * ddn = 1- cos(a1) * cos(a1) = sin(a1) * sin(a1)
        // 1 - nnt * nnt * (1 - ddn * ddn) = 1 - sin(a2) * sin(a2) = cos(2a2)
        if ((cos2t = 1 - nnt * nnt * (1 - ddn * ddn)) < epsilon)    // Total internal reflection 
            finalColor += objectColor * computePathTracing(baseGroup, reflectRay, backGroundColor, depths);

        Vector3d refractDirection = (ray.getDirection() * nnt - N * ((into ? 1 : -1) * (ddn * nnt + sqrt(cos2t)))).normalized();
        double  a = nt - nc, b = nt + nc, 
                R0 = a * a / (b * b), 
                c = 1 - (into ? -ddn : Vector3d::dot(N, refractDirection));
        double  Re = R0 + (1 - R0) * c * c * c * c * c, 
                Tr = 1 - Re, P = .25 + .5 * Re, 
                RP = Re / P, TP = Tr / (1 - P);
        finalColor += objectColor * ((depths > 2) ? 
            (rand0_1() < P ?   // Russian roulette 
            computePathTracing(baseGroup, reflectRay, backGroundColor, depths) * RP :
            computePathTracing(baseGroup, Ray(hitPoint, refractDirection), backGroundColor, depths) * TP) :
            computePathTracing(baseGroup, reflectRay, backGroundColor, depths) * Re +
            computePathTracing(baseGroup, Ray(hitPoint, refractDirection), backGroundColor, depths) * Tr);
    }

    return finalColor;
}

