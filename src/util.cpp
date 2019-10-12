#include "util.hpp"
#include <random>

// no idea
thread_local std::mt19937 rng(2019);

glm::vec3 random_unit_sphere()
{
    std::uniform_real_distribution dist(-1.f, 1.f);
    glm::vec3 p;
    do {
        p.x = dist(rng);
        p.y = dist(rng);
        p.z = dist(rng);
    } while (glm::length(p) >= 0.999f);
    return glm::normalize(p);
}

glm::vec3 random_unit_hemisphere(glm::vec3 n)
{
    glm::vec3 p;
    do {
        p = random_unit_sphere();
    } while (glm::dot(p, n) <= 0.f);
    return p;
}

glm::vec3 random_unit_disk()
{
	std::uniform_real_distribution dist(-1.f, 1.f);
	glm::vec3 p;
	p.z = 0.f;
	do {
		p.x = dist(rng);
		p.y = dist(rng);
	} while (glm::length(p) >= 0.999f);
	return p;
}

std::pair<glm::vec3, glm::vec3> plane_onb_from_normal(glm::vec3 n)
{
	glm::vec3 v(1.f, 0.f, 0.f);
	if (dot(v, n) >= 0.9) v = glm::vec3(0.f, 1.f, 0.f);
	glm::vec3 a = glm::cross(n, v);
	glm::vec3 b = glm::cross(n, a);
	return std::make_pair(a, b);
}

float random_between(float a, float b)
{
    std::uniform_real_distribution dist(a, b);
    return dist(rng);
}
