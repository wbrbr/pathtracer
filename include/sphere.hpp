#ifndef SPHERE_HPP
#define SPHERE_HPP
#include "ray.hpp"
#include <optional>

struct IntersectionData
{
    float t;
    vec3 normal;
};
class Sphere
{
public:
    vec3 c; // center
    float r; // radius

    Sphere(vec3 c, float r);
    std::optional<IntersectionData> intersects(Ray ray);
    vec3 normal(vec3 p);
};
#endif
