#ifndef SPHERE_HPP
#define SPHERE_HPP
#include "material.hpp"
#include "ray.hpp"
#include "shape.hpp"
#include <optional>

class Sphere : public Shape {
public:
    glm::vec3 c; // center
    float r; // radius
    MaterialID material;

    Sphere(glm::vec3 c, float r, MaterialID material);
    Box boundingBox();
    std::optional<IntersectionData> intersects(Ray ray);
    glm::vec3 normal(glm::vec3 p);
};
#endif
