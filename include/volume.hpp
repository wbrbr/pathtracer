#ifndef VOLUME_HPP
#define VOLUME_HPP
#include "glm/vec3.hpp"

class Volume
{
public:
    virtual float sampleDistance() = 0;
    virtual glm::vec3 samplePhase(glm::vec3 dir) = 0;
};

// TODO: make sigma_t / sigma_s colors
class HomogeneousVolume: public Volume
{
public:
    HomogeneousVolume(float g, float sigma_t, float sigma_s);
    float sampleDistance();
    glm::vec3 samplePhase(glm::vec3 dir);

private:
    float g, sigma_t, sigma_s;
};
#endif
