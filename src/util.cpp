#include "util.hpp"
#include <random>
#include <iostream>

// no idea
thread_local std::mt19937 rng(2019);

glm::vec3 random_unit_sphere()
{
    std::uniform_real_distribution<double> dist(-1.f, 1.f);
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
	std::uniform_real_distribution<double> dist(-1.f, 1.f);
	glm::vec3 p;
	p.z = 0.f;
	do {
		p.x = dist(rng);
		p.y = dist(rng);
	} while (glm::length(p) >= 0.999f);
	return p;
}

// Building an Orthonormal Basis, Revisited
// http://jcgt.org/published/0006/01/01/paper.pdf
std::pair<glm::vec3, glm::vec3> plane_onb_from_normal(glm::vec3 n)
{
    if (n.z < 0.) {
        const float a = 1.f / (1.f - n.z);
        const float b = n.x * n.y * a;
        glm::vec3 b1(1.f - n.x * n.x * a, -b, n.x);
        glm::vec3 b2(b, n.y * n.y * a - 1.f, -n.y);
        return std::make_pair(b1, b2);
    } else {
        const float a = 1.f / (1.f + n.z);
        const float b = -n.x * n.y * a;
        glm::vec3 b1(1.f - n.x * n.x * a, b, -n.x);
        glm::vec3 b2(b, 1.f - n.y * n.y * a, -n.y);
        return std::make_pair(b1, b2);
    }
}

float random_between(float a, float b)
{
    std::uniform_real_distribution<double> dist(a, b);
    return dist(rng);
}

// https://pharr.org/matt/blog/2019/02/27/triangle-sampling-1.html
glm::vec3 uniformSampleTriangle(Triangle& triangle)
{
    std::uniform_real_distribution<double> dist(0., 1.);
    float su0 = sqrt(dist(rng));
    float b0 = 1.f - su0;
    float b1 = dist(rng) * su0;

    glm::vec3 v0 = triangle.tm->getVertex(triangle.i0);
    glm::vec3 v1 = triangle.tm->getVertex(triangle.i1);
    glm::vec3 v2 = triangle.tm->getVertex(triangle.i2);

    assert(b0 >= 0);
    assert(b1 >= 0);

    return b0 * v0 + b1 * v1 + (1.f - b0 - b1) * v2;
}

int random_int(int a, int b)
{
    std::uniform_int_distribution<int> dist(a, b);
    return dist(rng);
}
