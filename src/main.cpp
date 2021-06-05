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
    for (int i = 0; i <= bounces; i++) {
        if (inter) {
            if (i == 0)
                L += throughput * inter->material->emitted();

            glm::vec3 Ld = world.directLighting(ray, *inter);

            L += throughput * Ld;

            assert(inter->material != nullptr);
            PDF* pdf = inter->material->getPDF(inter->normal);

            glm::vec3 new_dir = pdf->sample();
            float p = pdf->value(new_dir);

            glm::vec3 f = inter->material->eval(-ray.d, new_dir, inter->normal);

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


std::pair<World, Camera> suzanne()
{
    World world;
    loadObj("suzanne.obj", &world, new DiffuseMaterial(glm::vec3(.7, .4, .3)));
    // world.add(new Sphere(glm::vec3(0., 1., 0.), 1., new DiffuseMaterial(glm::vec3(1., 1.,1.))));
    world.envlight = std::make_unique<ConstantEnvLight>(glm::vec3(.5f,.6f,.7f));
    Camera cam(glm::vec3(0.f,0.f,4.f), glm::vec3(0.f, 0.f, 0.f), 30.f * M_PI / 180.f);
    return std::make_pair(std::move(world), cam);
}

std::pair<World, Camera> cornellBox()
{
    World world;
    loadObj("meshes/cornell.obj", &world, new DiffuseMaterial(glm::vec3(.7, .7, .7)));
    world.envlight = std::make_unique<ConstantEnvLight>(glm::vec3(1, 1, 1));
    //world.envlight = std::make_unique<EnvLight>("bg.hdr");
    glm::vec3 cam_pos(0.f, 1.f, 4.f);
    Camera cam(cam_pos, glm::vec3(0.f, 1.f, 0.f), 30.f * M_PI / 180.f);
    // return std::make_pair(world, cam);
    return std::make_pair(std::move(world), cam);
}

std::pair<World, Camera> furnace()
{
    World world;
    // world.add(new Sphere(glm::vec3(0., 0., 0.), .5f, new DiffuseMaterial(glm::vec3(1., 1., 1.))));
    world.add(std::make_unique<Sphere>(glm::vec3(0., 0., 0.), .5f, new MetalMaterial(glm::vec3(1., 1., 1.), 1.)));
    // world.envlight = std::make_unique<EnvLight>("bg.hdr");
    world.envlight = std::make_unique<ConstantEnvLight>(glm::vec3(.6, .6, .6));
    Camera cam(glm::vec3(0., 0., 3.), glm::vec3(0., 0., 0.), 30.f * M_PI / 180.);
    return std::make_pair(std::move(world), cam);
}

World test()
{
    World world;
    world.add(std::make_unique<Sphere>(glm::vec3(0), .194f, new DiffuseMaterial(glm::vec3(.7, .7, .7))));
    world.envlight = std::make_unique<HDREnvLight>("bg.hdr");
    return world;
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
