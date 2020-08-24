#include "light.hpp"
#include "util.hpp"
#include <iostream>
#include <fstream>

EnvLight::EnvLight(std::string path): tex(path)
{
}

std::pair<glm::vec3, float> EnvLight::sampleDirection(glm::vec3 n)
{
    glm::vec3 disk = random_unit_disk();
    auto basis = plane_onb_from_normal(n);
    float z = sqrt(1.f - disk.x * disk.x - disk.y * disk.y);
    glm::vec3 v = disk.x * basis.first + disk.y * basis.second + z * n;
    return std::make_pair(v, z * M_1_PI);
}

glm::vec3 EnvLight::emitted(glm::vec3 dir)
{
    // return glm::vec3(.6, .7, .8);
    float phi = std::atan2(dir.z, dir.x) * .5f * M_1_PI;
    if (phi < 0) phi += 1.;

    float theta = std::acos(dir.y) * M_1_PI;
    
    return tex.get(phi, theta);
}
