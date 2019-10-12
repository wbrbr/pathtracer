#include "ray.hpp"

Ray::Ray(glm::vec3 o, glm::vec3 d): o(o), d(glm::normalize(d))
{
}

glm::vec3 Ray::at(float t)
{
    return o + t*d;
}

Ray Ray::from_to(glm::vec3 o, glm::vec3 target)
{
    glm::vec3 direction = target - o;
    return Ray(o, direction);
}
