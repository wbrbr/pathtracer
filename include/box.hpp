#ifndef BOX_HPP
#define BOX_HPP
#include "ray.hpp"

class Box
{
public:
    Box();
    Box(glm::vec3 min, glm::vec3 max);
    glm::vec3 getMin();
    glm::vec3 getMax();
    glm::vec3 getCenter();
    Box add(Box box);
    bool intersects(Ray ray);

private:
    glm::vec3 bounds[2];
    glm::vec3 center;
};
#endif
