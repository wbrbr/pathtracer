#include "pdf.hpp"
#include "util.hpp"
#include <cmath>

PDF::~PDF()
{
}

UniformHemispherePDF::UniformHemispherePDF(glm::vec3 n)
{
    this->n = n;
}

float UniformHemispherePDF::value(glm::vec3 v)
{
    if (dot(v, n) < 0.f) return 0.f;
    return .5f * M_1_PI;
}

glm::vec3 UniformHemispherePDF::sample()
{
    return random_unit_hemisphere(n);
}

CosineHemispherePDF::CosineHemispherePDF(glm::vec3 n)
{
    this->n = n;
}

float CosineHemispherePDF::value(glm::vec3 v)
{
    float cosTheta = dot(n, v);
    return cosTheta < 0.f ? 0.f : cosTheta * M_1_PI;
}

glm::vec3 CosineHemispherePDF::sample()
{
    glm::vec3 disk = random_unit_disk();
    auto basis = plane_onb_from_normal(n);
    float z = sqrt(1.f - disk.x * disk.x - disk.y * disk.y);
    glm::vec3 v = disk.x * basis.first + disk.y * basis.second + z * n;
    return v;
}
