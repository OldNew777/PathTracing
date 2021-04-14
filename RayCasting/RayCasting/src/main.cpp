#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <iostream>
#include <time.h>
#include <random>
#include <string>

#include "scene_parser.hpp"
#include "image.hpp"
#include "camera.hpp"
#include "group.hpp"
#include "light.hpp"
#include "PathTracing.h"
#include "vecmath.h"

#include <Windows.h>


using namespace std;

int main(int argc, char *argv[]) {
    srand(time(0));

    for (int argNum = 1; argNum < argc; ++argNum) {
        std::cout << "Argument " << argNum << " is: " << argv[argNum] << std::endl;
    }

    if (argc != 3 && argc != 4) {
        cout << "Usage: ./bin/PA1 <input scene file> <output bmp file> (<samples>)" << endl;
        return 1;
    }
    string inputFile = argv[1];
    string outputFile = argv[2];  // only bmp is allowed.

    // 超采样次数
    int samples = 10;
    if (argc > 3)
        samples = max(1, atoi(argv[3]) / 4);

    // First, parse the scene using SceneParser.
    SceneParser sceneparser(&(inputFile[0]));

    // Then loop over each pixel in the image, shooting a ray
    // through that pixel and finding its intersection with
    // the scene.  Write the color at the intersection to that
    // pixel in your output image.
    Vector3d backGroundColor = sceneparser.getBackgroundColor();
    Camera* camera = sceneparser.getCamera();

    Image outputImage(camera->getWidth(), camera->getHeight());

    int startTime = time(0);

    Vector3d finalColor;
    #pragma omp parallel for schedule(dynamic, 1) private(finalColor)       // OpenMP 
    for (int x = 0; x < camera->getWidth(); ++x) {
        fprintf(stderr, "\rRendering (%d spp) %5.2f%%", samples * 4, 100. * x / (double(camera->getWidth()) - 1));
        for (int y = 0; y < camera->getHeight(); ++y) {
            // 超采样抗锯齿4X
            finalColor = Vector3d::ZERO;
            for (int interfere_y_AntiAliasing = 0; interfere_y_AntiAliasing < 2; ++interfere_y_AntiAliasing) {
                for (int interfere_x_AntiAliasing = 0; interfere_x_AntiAliasing < 2; ++interfere_x_AntiAliasing) {
                    // 重复发射samples条光线，因为漫反射的随机性
                    for (int times = 0; times < samples; ++times) {
                        // calculate the current pixel (x, y) 's camRay
                        Ray camRay = camera->generateRay(Vector2d(x, y), Vector2d(interfere_x_AntiAliasing, interfere_y_AntiAliasing));
                        finalColor += computePathTracing(sceneparser.getGroup(), camRay, backGroundColor, 0);
                    }
                }
            }
            outputImage.SetPixel(x, y, finalColor / double(4 * samples));
        }
    }

    outputImage.SaveBMP(&(outputFile[0]));

    cout << "\nHello! Computer Graphics!" << endl;
    cout << "Time consuming : " << time(0) - startTime << " s\n";
    return 0;
}

