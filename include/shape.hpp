#ifndef SHAPE_HPP
#define SHAPE_HPP
#include "box.hpp"
#include "glm/mat4x4.hpp"
#include "ray.hpp"
#include <optional>

class Material;

struct IntersectionData {
    float t;
    glm::vec3 normal;
    Material* material;

    IntersectionData()
    {
        material = nullptr;
    }
};

class Shape {
public:
    // virtual Box boundingBox() = 0;
    virtual std::optional<IntersectionData> intersects(Ray ray) = 0;
};

class TransformedShape : public Shape {
public:
    TransformedShape(Shape* s, glm::mat4 transform);
    std::optional<IntersectionData> intersects(Ray ray);

private:
    Shape* shape;
    glm::mat4 invTransform;
};

#endif
