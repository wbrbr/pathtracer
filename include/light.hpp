#ifndef LIGHT_HPP
#define LIGHT_HPP
#include <string>
#include <glm/vec3.hpp>
#include <utility>

class EnvLight
{
public:
    EnvLight(std::string path);
    std::pair<glm::vec3, float> sampleDirection(glm::vec3 n);
    glm::vec3 emitted(glm::vec3 dir);
};
#endif
