#ifndef LIGHT_HPP
#define LIGHT_HPP
#include "imagetexture.hpp"
#include "shape.hpp"
#include <glm/vec3.hpp>
#include <random>
#include <string>
#include <utility>
#include <vector>

class Light {
public:
    virtual ~Light() = default;
    virtual glm::vec3 samplePosition(const IntersectionData& inter, float& pdf, glm::vec3& emitted) = 0;
    virtual bool isDirectionalLight()
    {
        return false;
    }
};

class PointLight : public Light {
public:
    PointLight(glm::vec3 location, glm::vec3 irradiance);
    virtual ~PointLight() = default;
    virtual glm::vec3 samplePosition(const IntersectionData& inter, float& pdf, glm::vec3& emitted) override;

private:
    glm::vec3 location;
    glm::vec3 irradiance;
};

class DirectionalLight : public Light {
public:
    DirectionalLight(glm::vec3 direction, glm::vec3 radiance);
    virtual ~DirectionalLight() = default;
    virtual glm::vec3 samplePosition(const IntersectionData& inter, float& pdf, glm::vec3& emitted) override;
    virtual bool isDirectionalLight() override;

private:
    glm::vec3 direction;
    glm::vec3 radiance;
};

class EnvLight {
public:
    virtual ~EnvLight() {};
    virtual std::pair<glm::vec3, float> sampleDirection(glm::vec3 n) = 0;
    virtual glm::vec3 emitted(glm::vec3 dir) = 0;
};

class HDREnvLight : public EnvLight {
public:
    HDREnvLight(std::string path);
    std::pair<glm::vec3, float> sampleDirection(glm::vec3 n);
    glm::vec3 emitted(glm::vec3 dir);

private:
    ImageTexture tex;
    std::discrete_distribution<unsigned int> dist_y;
    std::vector<std::discrete_distribution<unsigned int>> dist_cond;
    std::mt19937 gen;
};

class ConstantEnvLight : public EnvLight {
public:
    ConstantEnvLight(glm::vec3 color);
    std::pair<glm::vec3, float> sampleDirection(glm::vec3 n);
    glm::vec3 emitted(glm::vec3 dir);

private:
    glm::vec3 color;
};
#endif
