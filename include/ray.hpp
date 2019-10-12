#ifndef RAY_HPP
#define RAY_HPP
#include "glm/glm.hpp"

class Ray
{
public:
    glm::vec3 o; // origin
    glm::vec3 d; // direction

    Ray(glm::vec3 o, glm::vec3 d);
    static Ray from_to(glm::vec3 o, glm::vec3 target);
    glm::vec3 at(float t);
};

#endif
