#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include <iostream>
#include <string>
#include <array>
#include <vector>
#include <random>
#include "ray.hpp"
#include "sphere.hpp"
#include "world.hpp"

std::mt19937 rng(2019);

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

vec3 random_unit_sphere()
{
    std::uniform_real_distribution dist(-1.f, 1.f);
    vec3 p;
    do {
        p.x = dist(rng);
        p.y = dist(rng);
        p.z = dist(rng);
    } while (p.norm_squared() >= 1.f);
    return p.normalized();
}

vec3 random_unit_hemisphere(vec3 n)
{
    vec3 p;
    do {
        p = random_unit_sphere();
    } while (dot(p, n) <= 0.f);
    return p;
}

vec3 color(World world, Ray ray)
{
    auto inter = world.intersects(ray);
    if (inter) {
        Ray new_ray(ray.at(inter->t) + 0.001f * inter->normal, random_unit_hemisphere(inter->normal));
        // Ray new_ray(ray.at(inter->t) + 0.001f * inter->normal, inter->normal);
        return 0.5f * color(world, new_ray);
        // return vec3(inter->normal.x / 2.f + 0.5f, inter->normal.y / 2.f + 0.5f, inter->normal.z / 2.f + 0.5f);
    } else {
        return lerp(vec3(1.f, 1.f, 1.f), vec3(0.5f, 0.7f, 1.0), ray.d.y / 2.f + 0.5f);
        // return vec3(0.5f, 0.7f, 1.f);
    }
}

int main() {

    World world;
    world.add(Sphere(vec3(0.f, 0.f, 0.f), 0.4f));
    world.add(Sphere(vec3(0.f, -100.4f, 0.f), 100.f));
    const int WIDTH = 500;
    const int HEIGHT = 500;
    const int SAMPLE_COUNT = 50;
    std::array<vec3, WIDTH*HEIGHT> pixels;
    std::uniform_real_distribution dist_x(-0.5f / (float)WIDTH, 0.5f / (float)WIDTH);
    std::uniform_real_distribution dist_y(-0.5f / (float)HEIGHT, 0.5f / (float)HEIGHT);
    for (int yi = 0; yi < HEIGHT; yi++)
    {
        float y =  -(((float)yi + 0.5f) * 2.f / (float)HEIGHT - 1.f);
        for (int xi = 0; xi < WIDTH; xi++)
        {
            float x = ((float)xi + 0.5f) * 2.f / (float)WIDTH - 1.f;

            vec3 c(0.f, 0.f, 0.f);
            for (int s = 0; s < SAMPLE_COUNT; s++)
            {
                float u = x + dist_x(rng);
                float v = y + dist_y(rng);
                Ray ray = Ray::from_to(vec3(0.f, 0.f, 1.f), vec3(u, v, 0.f));
                c += color(world, ray);
            }
            c /= (float)SAMPLE_COUNT;
            pixels[yi*WIDTH+xi] = c;
            // TODO: gamma correction
        }
    }
    write_png("out.png", WIDTH, HEIGHT, pixels.data());
}
