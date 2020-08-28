#ifndef LIGHT_HPP
#define LIGHT_HPP
#include <string>
#include <glm/vec3.hpp>
#include <utility>
#include <vector>
#include <random>
#include "imagetexture.hpp"

class EnvLight
{
public:
    EnvLight(std::string path);
    std::pair<glm::vec3, float> sampleDirection(glm::vec3 n);
    glm::vec3 emitted(glm::vec3 dir);

private:
    ImageTexture tex;
    std::discrete_distribution<unsigned int> dist_y;
    std::vector<std::discrete_distribution<unsigned int>> dist_cond;
    std::mt19937 gen;
};
#endif
