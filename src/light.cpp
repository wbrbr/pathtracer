#include "light.hpp"
#include "util.hpp"

EnvLight::EnvLight(std::string path)
{
    (void)path;
}

std::pair<glm::vec3, float> EnvLight::sampleDirection(glm::vec3 n)
{
    return std::make_pair(random_unit_hemisphere(n), .5f * M_1_PI);
}

glm::vec3 EnvLight::emitted(glm::vec3 dir)
{
    return glm::vec3(.6, .7, .8);
}
