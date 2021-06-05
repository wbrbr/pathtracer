#include "light.hpp"
#include "stb_image_write.h"
#include "util.hpp"
#include <algorithm>
#include <fstream>
#include <glm/gtx/norm.hpp>
#include <iostream>
#include <numeric>

PointLight::PointLight(glm::vec3 location, glm::vec3 irradiance)
    : location(location)
    , irradiance(irradiance)
{
}

glm::vec3 PointLight::samplePosition(const IntersectionData& inter, float& pdf, glm::vec3& emitted)
{
    pdf = 1;
    emitted = irradiance / glm::distance2(location, inter.p);
    return location;
}

float luminance(glm::vec3 c)
{
    return 0.212671f * c.r + 0.715160f * c.g + 0.072169f * c.b;
}

HDREnvLight::HDREnvLight(std::string path)
    : tex(path)
{
    std::vector<float> p_y, p_cond;

    for (unsigned int y = 0; y < tex.height(); y++) {
        float s = 0.;
        float v = (float)(tex.height() - 1 - y) + .5f;
        float theta = v * M_PI / (float)tex.height();
        float sinTheta = std::sin(theta);
        for (unsigned int x = 0; x < tex.width(); x++) {
            float v = luminance(tex.getPixel(x, y)) * sinTheta;
            p_cond.push_back(v);
            s += v;
        }
        p_y.push_back(s);
    }

    dist_y = std::discrete_distribution<unsigned int>(p_y.begin(), p_y.end());
    for (unsigned int y = 0; y < tex.height(); y++) {
        if (p_y.at(y) > 0.f) {
            dist_cond.push_back(std::discrete_distribution<unsigned int>(p_cond.begin() + y * tex.width(), p_cond.begin() + (y + 1) * tex.width()));
        } else {
            dist_cond.push_back(std::discrete_distribution<unsigned int>());
        }
    }
}

std::pair<glm::vec3, float> HDREnvLight::sampleDirection(glm::vec3 n)
{
    unsigned int y = dist_y(gen);
    unsigned int x = dist_cond.at(y)(gen);

    float py = dist_y.probabilities().at(y);
    float px = dist_cond.at(y).probabilities().at(x);
    float pixel_area = 1.f / ((float)(tex.width() * tex.height()));

    // probability of choosing specific point inside pixel. we divide by the area of the pixel
    float p = px * py / pixel_area;

    float u = (float)x / (float)tex.width();
    float v = ((float)(tex.height() - 1 - y) + .5f) / (float)tex.height();

    float theta = v * M_PI;
    float phi = u * 2.f * M_PI;
    float cosTheta = std::cos(theta);
    float sinTheta = std::sin(theta);

    float cosPhi = std::cos(phi);
    float sinPhi = std::sin(phi);
    p /= 2. * M_PI * M_PI * sinTheta;

    return std::make_pair(glm::vec3(sinTheta * cosPhi, cosTheta, sinTheta * sinPhi), p);
    /* glm::vec3 disk = random_unit_disk();
    auto basis = plane_onb_from_normal(n);
    float z = sqrt(1.f - disk.x * disk.x - disk.y * disk.y);
    glm::vec3 v = disk.x * basis.first + disk.y * basis.second + z * n;
    return std::make_pair(v, z * M_1_PI); */
}

glm::vec3 HDREnvLight::emitted(glm::vec3 dir)
{
    float phi = std::atan2(dir.z, dir.x) * .5f * M_1_PI;
    if (phi < 0)
        phi = std::min(0.9999f, phi + 1.f);

    assert(phi >= 0);
    assert(phi < 1);

    float theta = std::acos(-dir.y) * M_1_PI;
    assert(theta >= 0);
    if (theta >= 1.)
        theta = 0.9999f;
    assert(theta < 1);

    return .5f * tex.get(phi, theta);
}

ConstantEnvLight::ConstantEnvLight(glm::vec3 color)
    : color(color)
{
}

std::pair<glm::vec3, float> ConstantEnvLight::sampleDirection(glm::vec3 n)
{
    glm::vec3 v = cosine_unit_hemisphere(n);
    return std::make_pair(v, fmax(0., dot(n, v)) * M_1_PI);
}

glm::vec3 ConstantEnvLight::emitted(glm::vec3 dir)
{
    return color;
}