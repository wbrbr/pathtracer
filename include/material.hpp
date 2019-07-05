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
    virtual ScatterData scatter(Ray ray, IntersectionData inter) = 0;
};

class DiffuseMaterial: public Material
{
public:
    DiffuseMaterial(vec3 albedo);
    ScatterData scatter(Ray ray, IntersectionData inter);

private:
	vec3 albedo;

};

class MetalMaterial: public Material
{
public:
	MetalMaterial(vec3 albedo, float roughness);
	ScatterData scatter(Ray ray, IntersectionData inter);

private:
	vec3 albedo;
	float roughness;
};

#endif
