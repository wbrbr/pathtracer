#include "sphere.hpp"
#include <cmath>

Sphere::Sphere(glm::vec3 c, float r, MaterialID material)
    : c(c)
    , r(r)
    , material(material)
{
}

std::optional<IntersectionData> Sphere::intersects(Ray ray)
{
    glm::vec3 oc = ray.o - c;
    float a = dot(ray.d, ray.d);
    float b = 2.f * dot(oc, ray.d);
    float C = dot(oc, oc) - r * r;

    float discriminant = b * b - 4 * a * C;
    if (discriminant > 0) {
        IntersectionData inter;
        inter.material = material;
        inter.t = (-b - sqrt(discriminant)) / (2.f * a);
        if (inter.t <= 0.f) {
            inter.t = (-b + sqrt(discriminant)) / (2.f * a);
            if (inter.t <= 0.f)
                return {};
        }
        inter.normal = normal(ray.at(inter.t));
        return inter;
    } else {
        return {};
    }
}

glm::vec3 Sphere::normal(glm::vec3 p)
{
    return glm::normalize(p - c);
}

Box Sphere::boundingBox()
{
    return Box(c - glm::vec3(r, r, r), c + glm::vec3(r, r, r));
}
