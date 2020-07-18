#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include <iostream>
#include <string>
#include <array>
#include <vector>
#include <random>
#include <mutex>
#include <cassert>
#include "ray.hpp"
#include "sphere.hpp"
#include "world.hpp"
#include "util.hpp"
#include "trianglemesh.hpp"
#include "camera.hpp"
#include "pdf.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "json.hpp"
using json = nlohmann::json;

#ifdef NANCHECK
#include <fenv.h>
#endif

#define NUM_BOUNCES 4
#define NUM_SAMPLES 100
#define CLAMP_CONSTANT 100000.f

void write_png(std::string path, int w, int h, glm::vec3* pixels)
{
    std::vector<unsigned char> data;
    data.reserve(3*w*h);
    for (int i = 0; i < w*h; i++)
    {
        glm::vec3 clamped(fmin(pixels[i].x, 1.f), fmin(pixels[i].y, 1.f), fmin(pixels[i].z, 1.f));
        unsigned char r = (unsigned char)(pow(clamped.x, 0.4545)*255.99f);
        unsigned char g = (unsigned char)(pow(clamped.y, 0.4545)*255.99f);
        unsigned char b = (unsigned char)(pow(clamped.z, 0.4545)*255.99f);
        data.push_back(r);
        data.push_back(g);
        data.push_back(b);
    }
    stbi_write_png(path.c_str(), w, h, 3, data.data(), 0);
}

glm::vec3 color(World& world, Ray ray, int bounces, std::optional<IntersectionData> inter)
{
    if (inter) {
        assert(inter->material != nullptr);
        PDF* pdf = inter->material->getPDF(inter->normal);

        // not an emissive material. this actually isn't correct
        if (pdf != nullptr) {
            glm::vec3 direct = world.directLighting(ray, *inter);

            glm::vec3 new_dir = pdf->sample();
            float p = pdf->value(new_dir);

            Ray new_ray(ray.at(inter->t) + 0.001f * inter->normal, new_dir);
            glm::vec3 c(0);
            if (bounces > 0) {
                auto new_inter = world.intersects(new_ray);
                c = color(world, new_ray, bounces-1, new_inter);
            }
            glm::vec3 att = inter->material->eval(-ray.d, new_dir, inter->normal);
            delete pdf;
            return direct + dot(new_dir, inter->normal) * glm::vec3(c.x * att.x, c.y * att.y, c.z * att.z) / p;
        } else if (bounces == NUM_BOUNCES) { // no bounce
            return inter->material->emitted();
        } else { // purely emissive material, already taken into account by NEE
            return glm::vec3(0);
        }
    } else {
        //return glm::mix(glm::vec3(1.f, 1.f, 1.f), glm::vec3(0.5f, 0.7f, 1.0), ray.d.y / 2.f + 0.5f);
        //return glm::vec3(0.1f, .1f, .1f);
        return glm::vec3(0.f, 0.f, 0.f);
    }
}

/* glm::vec3 color(World world, Ray ray, int bounces)
{
    auto inter = world.intersects(ray);
    if (inter) {
        assert(inter->material != nullptr);
        PDF* pdf = inter->material->getPDF(inter->normal);
        glm::vec3 new_dir = pdf->sample();
        // glm::vec3 new_dir(0.f, 0.f, 1.f);
        glm::vec3 att = inter->material->eval(-ray.d, new_dir, inter->normal);
        delete pdf;
        return att;
    } else {
        return lerp(glm::vec3(1.f, 1.f, 1.f), glm::vec3(0.5f, 0.7f, 1.0), ray.d.y / 2.f + 0.5f);
    }
} */


World cornellBox()
{
    World world;
    loadObj("meshes/cornell.obj", &world, new DiffuseMaterial(glm::vec3(.7, .7, .7)));
    return world;
}

int main(int argc, char** argv) {
#ifdef NANCHECK
    feenableexcept(FE_INVALID | FE_OVERFLOW);
#endif
    if (argc < 4) {
        fprintf(stderr, "Usage: %s <width> <height> <out>", argv[0]);
        return 1;
    }
    const int WIDTH = atoi(argv[1]);
    const int HEIGHT = atoi(argv[2]);

    glm::vec3 cam_pos(0.f, 1.f, 5.f);
    float fov_y_rad = 40.f * M_PI / 180.f;
    float focal = 1.f / tan(fov_y_rad/2.f);

    World world = cornellBox();

	Camera cam(cam_pos, glm::vec3(0.f, 1.f, 0.f), focal);
    std::vector<glm::vec3> pixels;
	pixels.resize(WIDTH * HEIGHT);

    int done = 0;
    int percent = 0;
#pragma omp parallel for schedule(static,16)
    for (int i = 0; i < WIDTH*HEIGHT; i++)
    {
        int xi = i % WIDTH;
        int yi = i / WIDTH;
        glm::vec3 c(0.f, 0.f, 0.f);
        Ray ray = cam.getRay(xi, yi, WIDTH, HEIGHT);
        auto primary_intersection = world.intersects(ray);
        for (int s = 0; s < NUM_SAMPLES; s++)
        {
            c += glm::min(color(world, ray, NUM_BOUNCES, primary_intersection), glm::vec3(CLAMP_CONSTANT));
        }

#pragma omp atomic
        done++;

        if (done * 100 / (WIDTH*HEIGHT) > percent) {
            percent = done * 100 / (WIDTH*HEIGHT);
            std::cout << "Done: " << percent << "%" << std::endl;
        }
        c /= (float)NUM_SAMPLES;
        pixels[i] = c;
    }

    write_png(argv[3], WIDTH, HEIGHT, pixels.data());
}
