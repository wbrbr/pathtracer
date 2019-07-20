#include "material.hpp"
#include "util.hpp"
#include <cmath>

DiffuseMaterial::DiffuseMaterial(vec3 albedo): albedo(albedo)
{
}

PDF* DiffuseMaterial::getPDF(vec3 n)
{
    return new CosineHemispherePDF(n);
}

// Color
vec3 DiffuseMaterial::eval(vec3 wi, vec3 wo)
{
    return M_1_PI * albedo;
}

/* ScatterData DiffuseMaterial::scatter(Ray ray, IntersectionData inter)
{
    ScatterData sc;
    sc.attenuation = albedo;
    Ray new_ray(ray.at(inter.t) + 0.001f * inter.normal, random_unit_hemisphere(inter.normal));
    sc.scattered = new_ray;
    return sc;
} */

/* MetalMaterial::MetalMaterial(vec3 albedo, float roughness) : albedo(albedo), roughness(roughness)
{
}

ScatterData MetalMaterial::scatter(Ray ray, IntersectionData inter)
{
	vec3 reflected = reflect(ray.d, inter.normal);
	vec3 rand_disk = random_unit_disk();
	std::pair<vec3, vec3> basis = plane_onb_from_normal(inter.normal);
	vec3 offset = roughness * (rand_disk.x * basis.first + rand_disk.y * basis.second);
	ScatterData sc;
	sc.scattered = Ray(ray.at(inter.t) + 0.001f * inter.normal, reflected + offset);
	sc.attenuation = albedo;
	return sc;
} */
