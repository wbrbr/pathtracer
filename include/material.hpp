#ifndef MATERIAL_HPP
#define MATERIAL_HPP
#include <optional>
#include "ray.hpp"

class Material;

struct IntersectionData
{
    float t;
    vec3 normal;
    Material* material;
};

struct ScatterData
{
    vec3 attenuation;
    std::optional<Ray> scattered;
};

class Material
{
public:
    virtual std::optional<ScatterData> scatter(Ray ray, IntersectionData inter) = 0;
};

class DiffuseMaterial: public Material
{
public:
    vec3 albedo;
    
    DiffuseMaterial(vec3 albedo);
    ScatterData scatter(Ray ray, IntersectionData inter);
};

/* class MetalMaterial: public Material
{
public:
    vec3 albedo;
    
    std::optional<ScatterData> scatter(Ray ray, IntersectionData inter);
}; */
#endif
