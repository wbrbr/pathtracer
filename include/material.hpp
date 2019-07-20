#ifndef MATERIAL_HPP
#define MATERIAL_HPP
#include <optional>
#include "ray.hpp"
#include "shape.hpp"
#include "pdf.hpp"

struct ScatterData
{
    vec3 attenuation;
    std::optional<Ray> scattered;
};

class Material
{
public:
    // virtual ScatterData scatter(Ray ray, IntersectionData inter) = 0;
    virtual PDF* getPDF(vec3 n) = 0;
    virtual vec3 eval(vec3 wi, vec3 wo) = 0;
};

class DiffuseMaterial: public Material
{
public:
    DiffuseMaterial(vec3 albedo);
    PDF* getPDF(vec3 n);
    vec3 eval(vec3 wi, vec3 wo);

private:
	vec3 albedo;
};

/* class MetalMaterial: public Material
{
public:
	MetalMaterial(vec3 albedo, float roughness);
	ScatterData scatter(Ray ray, IntersectionData inter);

private:
	vec3 albedo;
	float roughness;
}; */

#endif
