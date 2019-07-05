#include "material.hpp"
#include "util.hpp"

DiffuseMaterial::DiffuseMaterial(vec3 albedo): albedo(albedo)
{
}

ScatterData DiffuseMaterial::scatter(Ray ray, IntersectionData inter)
{
    ScatterData sc;
    sc.attenuation = albedo;
    Ray new_ray(ray.at(inter.t) + 0.001f * inter.normal, random_unit_hemisphere(inter.normal));
    sc.scattered = new_ray;
    return sc;
}
