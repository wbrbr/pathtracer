#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include <iostream>
#include <string>
#include <array>
#include <vector>
#include <random>
#include <mutex>
#include <thread>
#include "ray.hpp"
#include "sphere.hpp"
#include "world.hpp"
#include "util.hpp"
#include "trianglemesh.hpp"
#include "camera.hpp"

// WARN: must divide the number of rows in the output image
#define NUM_THREADS 10

void write_png(std::string path, int w, int h, vec3* pixels)
{
    std::vector<unsigned char> data;
    data.reserve(3*w*h);
    for (int i = 0; i < w*h; i++)
    {
        data.push_back((unsigned char)(pow(pixels[i].x, 0.4545)*255.99f));
        data.push_back((unsigned char)(pow(pixels[i].y, 0.4545)*255.99f));
        data.push_back((unsigned char)(pow(pixels[i].z, 0.4545)*255.99f));
    }
    stbi_write_png(path.c_str(), w, h, 3, data.data(), 0);
}

vec3 color(World world, Ray ray)
{
    auto inter = world.intersects(ray);
    if (inter) {
        assert(inter->material != nullptr);
        auto sc = inter->material->scatter(ray, *inter);
        if (sc.scattered) {
            vec3 c = color(world, *sc.scattered);
            return vec3(sc.attenuation.x * c.x, sc.attenuation.y * c.y, sc.attenuation.z * c.z);
        } else {
            return vec3(0.f, 0.f, 0.f);
        }
        Ray new_ray(ray.at(inter->t) + 0.001f * inter->normal, random_unit_hemisphere(inter->normal));
        return 0.5f * color(world, new_ray);
    } else {
        return lerp(vec3(1.f, 1.f, 1.f), vec3(0.5f, 0.7f, 1.0), ray.d.y / 2.f + 0.5f);
    }
}

void trace_rays(int i, int width, int height, int sample_count, std::vector<vec3>& pixels, std::mutex& mut, World world, Camera cam)
{
    const int rows = height / NUM_THREADS;
    for (int yi = rows*i; yi < rows * (i+1); yi++)
    {
        for (int xi = 0; xi < width; xi++)
        {
            vec3 c(0.f, 0.f, 0.f);
            for (int s = 0; s < sample_count; s++)
            {
                Ray ray = cam.getRay(xi, yi, width, height);
                c += color(world, ray);
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
    World world;
    DiffuseMaterial mat(vec3(0.8f, 0.8f, 0.f));
    TriangleMesh tm("suzanne.obj", &mat);
    // world.add(new Sphere(vec3(0.f, 0.f, 0.f), 0.4f, new DiffuseMaterial(vec3(0.8f, 0.3f, 0.3f))));
	// world.add(new Sphere(vec3(0.8f, 0.f, 0.f), 0.4f, new MetalMaterial(vec3(0.8f, 0.6f, 0.2f), 1.f)));
    // world.add(new Sphere(vec3(0.f, -100.4f, 0.f), 100.f, new DiffuseMaterial(vec3(0.8f, 0.8f, 0.f))));
	world.add(&tm);
	Camera cam(vec3(0.f, 1.f, 1.f), vec3(0.f, 0.f, 0.f));
    const int WIDTH = 500;
    const int HEIGHT = 500;
    const int SAMPLE_COUNT = 10;
    std::vector<vec3> pixels;
	pixels.resize(WIDTH * HEIGHT);
    std::mutex pixels_mutex;

    std::array<std::thread, NUM_THREADS> threads;
    for (int i = 0; i < NUM_THREADS; i++)
    {
        threads[i] = std::thread(&trace_rays, i, WIDTH, HEIGHT, SAMPLE_COUNT, std::ref(pixels), std::ref(pixels_mutex), world, cam);
    }
    for (int i = 0; i < NUM_THREADS; i++)
    {
        threads[i].join();
    }

    write_png("out.png", WIDTH, HEIGHT, pixels.data());
}
