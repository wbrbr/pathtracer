#include "light.hpp"
#include "util.hpp"

EnvLight::EnvLight(std::string path)
{
    (void)path;
}

std::pair<glm::vec3, float> EnvLight::sampleDirection(glm::vec3 n)
{
    // return std::make_pair(random_unit_hemisphere(n), M_1_PI);
    glm::vec3 disk = random_unit_disk();
    auto basis = plane_onb_from_normal(n);
    float z = sqrt(1.f - disk.x * disk.x - disk.y * disk.y);
    glm::vec3 v = disk.x * basis.first + disk.y * basis.second + z * n;
    return std::make_pair(v, z * M_1_PI);
}

glm::vec3 EnvLight::emitted(glm::vec3 dir)
{
    return glm::vec3(.6, .7, .8);
}
