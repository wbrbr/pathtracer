#ifndef SPHERE_HPP
#define SPHERE_HPP
#include "ray.hpp"
#include "material.hpp"
#include <optional>

class Sphere
{
public:
    vec3 c; // center
    float r; // radius
    Material* material;

    Sphere(vec3 c, float r, Material* material);
    std::optional<IntersectionData> intersects(Ray ray);
    vec3 normal(vec3 p);
};
#endif
