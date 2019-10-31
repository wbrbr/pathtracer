#include "ray.hpp"

Ray::Ray(glm::vec3 o, glm::vec3 d): o(o), d(glm::normalize(d))
{
    invd = glm::vec3(1.f / d.x, 1.f / d.y, 1.f / d.z);
    sign[0] = (invd.x < 0);
    sign[1] = (invd.y < 0);
    sign[2] = (invd.z < 0);
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
