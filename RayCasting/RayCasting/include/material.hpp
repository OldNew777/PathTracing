#ifndef MATERIAL_H
#define MATERIAL_H

#include <cassert>
#include <iostream>
#include <algorithm>

#include "Vector3d.h"
#include "image.hpp"
#include "ray.hpp"
#include "hit.hpp"


class Material {
public:

    explicit Material(char* textureFilename, 
                      const Vector3d &color, const Vector3d& emission_color = Vector3d::ZERO,
                      double refract_rate = 0, double specular_rate = 0) :
            color(color), emissionColor(emission_color), 
            refractRate(refract_rate), specularRate(specular_rate) {
        if (textureFilename[0] != 0) {
            texturePicture = Image::LoadTGA(textureFilename);
            width = texturePicture->Width();
            height = texturePicture->Height();
        }
    }
    
    virtual ~Material() {
        if (texturePicture != nullptr)
            delete texturePicture;
    }

    virtual Vector3d getColor() const { return color; }
    virtual Vector3d getEmissionColor() const { return emissionColor; }

    virtual double getRefractRate() const { return refractRate; }
    virtual double getSpecularRate() const { return specularRate; }

    virtual bool haveTexture() const { return (texturePicture != nullptr); }
    virtual Vector3d getTexture_uv(double u, double v) const {
        int i = u * width;//Çó³öÏñËØË÷Òý
        int j = (1 - v) * height - 0.001;

        if (i < 0) i = 0;
        if (i > width - 1) i = width - 1;
        if (j < 0) j = 0;
        if (j > height - 1) j = height - 1;

        return texturePicture->GetPixel(i, j);
    }
    virtual Vector3d getTexture_xy(double x, double y, double proportion) const {
        double width = this->width * proportion, height = this->height * proportion;
        if (x < 0) {     // -2999 % 1024 = -951
            x -= width * (double(int(x / width)) - 1);    // x = 1024 + (-951) = 73
        }
        while (x >= width)     // x = 1024 % 1024 = 0
            x -= width;

        if (y < 0)
            y -= height * (double(int(y / height)) - 1);
        while (y >= height)
            y -= height;

        return texturePicture->GetPixel(x / proportion, y / proportion);
    }

    Material& operator=(const Material& rm)
    {
        color = rm.color;
        emissionColor = rm.emissionColor;

        refractRate = rm.refractRate;
        specularRate = rm.specularRate;
    }

protected:
    Vector3d color;
    Vector3d emissionColor;

    double refractRate;
    double specularRate;

    Image* texturePicture = nullptr;
    int width, height;
};


#endif // MATERIAL_H
