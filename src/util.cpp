#include "util.hpp"
#include <random>

std::mt19937 rng(2019);

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


float random_between(float a, float b)
{
    std::uniform_real_distribution dist(a, b);
    return dist(rng);
}
