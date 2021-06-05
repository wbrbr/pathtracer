#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "camera.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/string_cast.hpp"
#include "pbrt_parser.hpp"
#include "pdf.hpp"
#include "ray.hpp"
#include "sphere.hpp"
#include "stb_image_write.h"
#include "trianglemesh.hpp"
#include "util.hpp"
#include "world.hpp"
#include <array>
#include <cassert>
#include <iostream>
#include <mutex>
#include <random>
#include <string>
#include <utility>
#include <vector>

#ifdef NANCHECK
#include <fenv.h>
#endif

#define NUM_BOUNCES 10
#define NUM_SAMPLES 100
#define CLAMP_CONSTANT 100000.f

void write_png(std::string path, int w, int h, glm::vec3* pixels)
{
    std::vector<unsigned char> data;
    data.reserve(3 * w * h);
    for (int i = 0; i < w * h; i++) {
        glm::vec3 clamped(fmin(pixels[i].x, 1.f), fmin(pixels[i].y, 1.f), fmin(pixels[i].z, 1.f));
        unsigned char r = (unsigned char)(pow(clamped.x, 0.4545) * 255.99f);
        unsigned char g = (unsigned char)(pow(clamped.y, 0.4545) * 255.99f);
        unsigned char b = (unsigned char)(pow(clamped.z, 0.4545) * 255.99f);
        data.push_back(r);
        data.push_back(g);
        data.push_back(b);
    }
    stbi_write_png(path.c_str(), w, h, 3, data.data(), 0);
}

glm::vec3 color(World& world, Ray primary_ray, int bounces, std::optional<IntersectionData> primary_inter)
{
    auto inter = primary_inter;
    Ray ray = primary_ray;
    glm::vec3 L(0.f);
    glm::vec3 throughput(1.f);
    Material* mat;

    for (int i = 0; i <= bounces; i++) {
        if (inter) {
            mat = world.getMaterial(inter->material);
            if (i == 0)
                L += throughput * mat->emitted();

            glm::vec3 Ld = world.directLighting(ray, *inter);

            L += throughput * Ld;

            assert(inter->material != nullptr);
            PDF* pdf = mat->getPDF(inter->normal);

            glm::vec3 new_dir = pdf->sample();
            float p = pdf->value(new_dir);

            glm::vec3 f = mat->eval(-ray.d, new_dir, inter->normal);

            ray = Ray(ray.at(inter->t) + 0.001f * inter->normal, new_dir);

            delete pdf;
            throughput *= glm::dot(new_dir, inter->normal) * f / p;

            // if (i > 2) {
            //     float q = std::max(.05f, 1.f - throughput.length());
            //     if (random_between(0.f, 1.f) < q) break;
            //     throughput /= (1.f - q);
            // }

            inter = world.intersects(ray);
        } else {
            if (i == 0 && world.envlight)
                L += world.envlight->emitted(ray.d);
            break;
        }
    }
    return L;
}

int main(int argc, char** argv)
{
#ifdef NANCHECK
    feenableexcept(FE_INVALID | FE_OVERFLOW);
#endif
    if (argc < 4) {
        fprintf(stderr, "Usage: %s <pbrt file> <width> <height> <out>", argv[0]);
        return 1;
    }
    const int WIDTH = atoi(argv[2]);
    const int HEIGHT = atoi(argv[3]);

    std::string path(argv[1]);
    
    //auto pair = cornellBox();
    //auto pair = furnace();
    //auto pair = suzanne();
    auto pair = parsePbrt(path);
    World world = std::move(pair.first);
    Camera cam = std::move(pair.second);
    // World world = suzanne();
    // World world = furnace();
    // World world = test();

    // Camera cam(cam_pos, glm::vec3(0.f, 1.f, 0.f), fov_y_rad);
    // Camera cam(cam_pos, glm::vec3(0.f, 0.f, 0.f), fov_y_rad);
    std::vector<glm::vec3> pixels;
    pixels.resize(WIDTH * HEIGHT);

    int done = 0;
    int percent = 0;
#pragma omp parallel for schedule(static, 16)
    for (int i = 0; i < WIDTH * HEIGHT; i++) {
        int xi = i % WIDTH;
        int yi = i / WIDTH;
        glm::vec3 c(0.f, 0.f, 0.f);
        for (int s = 0; s < NUM_SAMPLES; s++) {
            Ray ray = cam.getRay(xi, yi, WIDTH, HEIGHT);
            auto primary_intersection = world.intersects(ray);
            c += glm::min(color(world, ray, NUM_BOUNCES, primary_intersection), glm::vec3(CLAMP_CONSTANT));
        }

#pragma omp atomic
        done++;

        if (done * 100 / (WIDTH * HEIGHT) > percent) {
            percent = done * 100 / (WIDTH * HEIGHT);
            std::cout << "Done: " << percent << "%" << std::endl;
        }
        c /= (float)NUM_SAMPLES;
        pixels[i] = c;
    }

    //write_png(argv[3], WIDTH, HEIGHT, pixels.data());
    stbi_write_hdr(argv[4], WIDTH, HEIGHT, 3, (float*)pixels.data());
}
