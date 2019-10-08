#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include <iostream>
#include <string>
#include <array>
#include <vector>
#include <random>
#include <mutex>
#include <thread>
#include <cassert>
#include "ray.hpp"
#include "sphere.hpp"
#include "world.hpp"
#include "util.hpp"
#include "trianglemesh.hpp"
#include "camera.hpp"
#include "pdf.hpp"

//#include <fenv.h>

// WARN: must divide the number of rows in the output image
#define NUM_THREADS 10
#define NUM_BOUNCES 10
#define NUM_SAMPLES 500

void write_png(std::string path, int w, int h, vec3* pixels)
{
    std::vector<unsigned char> data;
    data.reserve(3*w*h);
    for (int i = 0; i < w*h; i++)
    {
        vec3 clamped(fmin(pixels[i].x, 1.f), fmin(pixels[i].y, 1.f), fmin(pixels[i].z, 1.f));
        unsigned char r = (unsigned char)(pow(clamped.x, 0.4545)*255.99f);
        unsigned char g = (unsigned char)(pow(clamped.y, 0.4545)*255.99f);
        unsigned char b = (unsigned char)(pow(clamped.z, 0.4545)*255.99f);
        data.push_back(r);
        data.push_back(g);
        data.push_back(b);
    }
    stbi_write_png(path.c_str(), w, h, 3, data.data(), 0);
}

vec3 color(World world, Ray ray, int bounces)
{
    auto inter = world.intersects(ray);
    if (inter) {
        assert(inter->material != nullptr);
        PDF* pdf = inter->material->getPDF(inter->normal);
        vec3 new_dir = pdf->sample();
        Ray new_ray(ray.at(inter->t) + 0.001f * inter->normal, new_dir);
        vec3 c = (bounces > 0) ? color(world, new_ray, bounces-1) : vec3(0.f, 0.f, 0.f);
        vec3 att = inter->material->eval(-ray.d, new_dir, inter->normal);
        float p = pdf->value(new_dir);
        delete pdf;
        return dot(new_dir, inter->normal) * vec3(c.x * att.x, c.y * att.y, c.z * att.z) / p;
    } else {
        return lerp(vec3(1.f, 1.f, 1.f), vec3(0.5f, 0.7f, 1.0), ray.d.y / 2.f + 0.5f);
    }
}

/*vec3 color(World world, Ray ray)
{
    auto inter = world.intersects(ray);
    if (inter) {
        assert(inter->material != nullptr);
        PDF* pdf = inter->material->getPDF(inter->normal);
        // vec3 new_dir = pdf->sample();
        vec3 new_dir(0.f, 0.f, 1.f);
        vec3 att = inter->material->eval(-ray.d, new_dir, inter->normal);
        delete pdf;
        return att;
    } else {
        return lerp(vec3(1.f, 1.f, 1.f), vec3(0.5f, 0.7f, 1.0), ray.d.y / 2.f + 0.5f);
    }
}*/

void trace_rays(int i, int width, int height, int sample_count, std::vector<vec3>& pixels, std::mutex& mut, World world, Camera cam)
{
    const int rows = height / NUM_THREADS;
    for (int yi = rows*i; yi < rows * (i+1); yi++)
    {
        for (int xi = 0; xi < width; xi++)
        {
            char msg[10];
            snprintf(msg, 10, "%d", yi*width+xi);
            vec3 c(0.f, 0.f, 0.f);
            for (int s = 0; s < sample_count; s++)
            {
                Ray ray = cam.getRay(xi, yi, width, height);
                c += color(world, ray, NUM_BOUNCES);
            }
            c /= (float)sample_count;
            mut.lock();
            pixels[yi*width+xi] = c;
            mut.unlock();
        }
    }
}

int main() {
    /* Box b(vec3(0.f, 0.f, 0.f), vec3(1.f, 1.f, 1.f));
    Ray r(vec3(0.5f, 0.5f, -1.f), vec3(0.5f, 0.5f, 1.f));
    std::cout << b.intersects(r) << std::endl;
    return 0; */
    //feenableexcept(FE_INVALID | FE_OVERFLOW);
    World world;
    DiffuseMaterial mat(vec3(0.8f, 0.8f, 0.f));
    // MetalMaterial met(vec3(0.9f, 0.9f, 0.9f), 0.05f);
    MetalMaterial met(vec3(1.f, 1.f, 1.f), 0.1f);
    // TriangleMesh tm("cube.obj", &mat);
	// world.add(&tm);
    world.add(new Sphere(vec3(0.f, 0.f, 0.f), 0.4f, new DiffuseMaterial(vec3(0.8f, 0.3f, 0.3f))));
	world.add(new Sphere(vec3(0.8f, 0.f, 0.f), 0.4f, &met));
    world.add(new Sphere(vec3(0.f, -100.4f, 0.f), 100.f, new DiffuseMaterial(vec3(0.8f, 0.8f, 0.f))));
	Camera cam(vec3(0.5f, 1.f, 1.f), vec3(0.f, 0.f, 0.f));
    const int WIDTH = 500;
    const int HEIGHT = 500;
    std::vector<vec3> pixels;
	pixels.resize(WIDTH * HEIGHT);
    std::mutex pixels_mutex;

    std::array<std::thread, NUM_THREADS> threads;
    for (int i = 0; i < NUM_THREADS; i++)
    {
        threads[i] = std::thread(&trace_rays, i, WIDTH, HEIGHT, NUM_SAMPLES, std::ref(pixels), std::ref(pixels_mutex), world, cam);
    }
    for (int i = 0; i < NUM_THREADS; i++)
    {
        threads[i].join();
    }

    write_png("out.png", WIDTH, HEIGHT, pixels.data());
}
