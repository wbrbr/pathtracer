#include "shape.hpp"
#include <iostream>
#include "glm/gtx/string_cast.hpp"

TransformedShape::TransformedShape(Shape* s, glm::mat4 transform)
{
    shape = s; 
    invTransform = glm::inverse(transform);
}

std::optional<IntersectionData> TransformedShape::intersects(Ray ray)
{
    glm::vec3 p1 = invTransform * glm::vec4(ray.o, 1.f);
    glm::vec3 p2 = invTransform * glm::vec4(ray.o + ray.d, 1.f);
    Ray transformedRay = Ray::from_to(p1, p2);
    return shape->intersects(transformedRay);
}
