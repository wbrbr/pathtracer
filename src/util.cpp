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

vec3 random_unit_disk()
{
	std::uniform_real_distribution dist(-1.f, 1.f);
	vec3 p;
	p.z = 0.f;
	do {
		p.x = dist(rng);
		p.y = dist(rng);
	} while (p.norm_squared() > 1.f);
	return p;
}

std::pair<vec3, vec3> plane_onb_from_normal(vec3 n)
{
	vec3 v(1.f, 0.f, 0.f);
	if (dot(v, n) >= 0.9) v = vec3(0.f, 1.f, 0.f);
	vec3 a = cross(n, v);
	vec3 b = cross(n, a);
	return std::make_pair(a, b);
}

float random_between(float a, float b)
{
    std::uniform_real_distribution dist(a, b);
    return dist(rng);
}
