#ifndef VOLUME_HPP
#define VOLUME_HPP
#include "ray.hpp"
#include <openvdb/openvdb.h>

class Volume
{
public:
    virtual float sampleDistance(Ray ray) = 0;
    virtual glm::vec3 samplePhase(glm::vec3 dir) = 0;
};

// TODO: make sigma_t / sigma_s colors
class HomogeneousVolume: public Volume
{
public:
    HomogeneousVolume(float g, float sigma_t, float albedo);
    float sampleDistance(Ray ray);
    glm::vec3 samplePhase(glm::vec3 dir);

private:
    float g, sigma_t, albedo;
};

class HeterogeneousVolume: public Volume
{
public:
    HeterogeneousVolume(float g, std::string vdb_path, float albedo);
    float sampleDistance(Ray ray);
    glm::vec3 samplePhase(glm::vec3 dir);

private:
    float g;
    float albedo;
    float sigma_hat;
    openvdb::FloatGrid::Ptr grid;
};
#endif
