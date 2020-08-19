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
#include "volume.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "json.hpp"
using json = nlohmann::json;
#include <openvdb/openvdb.h>

#ifdef NANCHECK
#include <fenv.h>
#endif

#define NUM_BOUNCES 10
#define NUM_SAMPLES 10
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

glm::vec3 color(World& world, Ray primary_ray, int bounces, std::optional<IntersectionData> primary_inter)
{
    auto inter = primary_inter;
    Ray ray = primary_ray;
    glm::vec3 L(0.f);
    glm::vec3 throughput(0.f);
    for (int i = 0; i < bounces; i++)
    {
        if (inter) {
            if (i == 0) L += throughput * inter->material->emitted();
            L += throughput * world.directLighting(ray, *inter);

            assert(inter->material != nullptr);
            PDF* pdf = inter->material->getPDF(inter->normal);

            glm::vec3 new_dir = pdf->sample();
            float p = pdf->value(new_dir);

            ray = Ray(ray.at(inter->t) + 0.001f * inter->normal, new_dir);

            glm::vec3 f = inter->material->eval(-ray.d, new_dir, inter->normal);
            delete pdf;
            throughput *= glm::dot(new_dir, inter->normal) * f / p;

            if (i > 2) {
                float q = std::max(.05f, 1.f - throughput.length());
                if (random_between(0.f, 1.f)) break;
                throughput /= (1.f - q);
            }
        } else {
            return glm::vec3(0.f, 0.f, 0.f);
        }
    }
    return L;
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

float sample_distance(float sigma_t, float sigma_hat)
{
    float t = 0;
    for (;;) {
        t -= log(1 - random_between(0., 1.)) / sigma_hat;
        if (random_between(0., 1.) < sigma_t / sigma_hat) {
            break;
        }
    }

    return t;
}

glm::vec3 volume(Ray ray, Volume& media)
{
    const glm::vec3 sky_color(.6, .7, .8);

    Sphere vol(glm::vec3(0.), 1.f, nullptr);

    auto inter = vol.intersects(ray);
    if (!inter) {
        return sky_color;
    }

    float tmin = inter->t;

    ray.o = ray.at(tmin) + 0.001f * ray.d;
    inter = vol.intersects(ray);
    if (!inter || inter->t < 0) {
        return sky_color;
    }

    float tmax = tmin + inter->t;

    float d = tmax - tmin;
    assert(d >= 0);

    // ratio tracking transmittance estimation
    // float tr = 1.f;
    // float t = 0.;
    // for (;;) {
    //     float rnd = random_between(0., 1.);
    //     t -= log(1 - rnd) / sigma_hat;
    //     if (t > d) break;
    //     float sigma_t = glm::length(ray.at(t));
    //     // float sigma_t = 1.;
    //     tr *= 1.f - sigma_t / sigma_hat;
    // }

    float sigma_hat = 1.3f;
    float albedo = 0.1;
    float throughput = 1.f;
    glm::vec3 L(0.f);


    for (int i = 0; i < 1; i++)
    {
        // float t = -log(1.f - random_between(0.f, 1.f)) / sigma_t;
        float t = media.sampleDistance(ray);

        if (t < d) {
            throughput *= albedo;
        } else {
            L += throughput * sky_color;
            break;
        }

        // throughput *= homo_sample(ray, d, new_pos);

        if (glm::length(throughput) == 0.) break;

        // glm::vec3 new_dir = random_unit_sphere();
        glm::vec3 new_dir = media.samplePhase(-ray.d);
        ray = Ray(ray.at(t), new_dir);
        assert(glm::length(ray.o) <= 1.);
        inter = vol.intersects(ray);
        // assert(inter); // we are inside the sphere so we should be colliding
        if (!inter) { // we are out of the volume (precision error)
            L += throughput * sky_color;
            break;
        }
        d = inter->t;

        // russian roulette
    }

    return L;
}


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

	Camera cam(cam_pos, glm::vec3(0.f, 0.f, 0.f), focal);
    std::vector<glm::vec3> pixels;
	pixels.resize(WIDTH * HEIGHT);

    int done = 0;
    int percent = 0;

    openvdb::initialize();
    // HomogeneousVolume media(0, sigma_t, .5);
    HeterogeneousVolume media(0, "smoke.vdb", .5);

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
            // c += glm::min(color(world, ray, NUM_BOUNCES, primary_intersection), glm::vec3(CLAMP_CONSTANT));
            c += volume(ray, media);
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
