#include "material.hpp"
#include "util.hpp"
#include <cmath>
#include <cassert>

DiffuseMaterial::DiffuseMaterial(vec3 albedo): albedo(albedo)
{
}

PDF* DiffuseMaterial::getPDF(vec3 n)
{
    return new CosineHemispherePDF(n);
}

// Color
vec3 DiffuseMaterial::eval(vec3 wi, vec3 wo, vec3 n)
{
    return M_1_PI * albedo;
}

MetalMaterial::MetalMaterial(vec3 albedo, float alpha): albedo(albedo), alpha(alpha)
{
}

PDF* MetalMaterial::getPDF(vec3 n)
{
    return new CosineHemispherePDF(n);
}

float GGXF(vec3 wi, vec3 hr)
{
    const float F0 = 0.04;
    float x = 1.f - dot(hr, wi);
    return F0 + (1.f - F0) * x * x * x * x * x;
}

// http://graphicrants.blogspot.com/2013/08/specular-brdf-reference.html
float GGXD(vec3 m, vec3 n, float alpha) 
{
    float cosTheta = dot(m, n);
    float tanTheta = (1.f - cosTheta*cosTheta) / cosTheta;
    float fac = (alpha * alpha + tanTheta * tanTheta);
    float res;
    if (cosTheta < 0) res = 0.f;
    else res = alpha * alpha * M_1_PI / (cosTheta * cosTheta * cosTheta * cosTheta * fac * fac);
    // assert(res >= 0.f);
    // assert(res <= 1.f);
    // assert(res * cosTheta <= 1.f);
    return res;
}

float GGXG(vec3 v, vec3 m, vec3 n, float alpha)
{
    assert(v.isNormalized());
    assert(m.isNormalized());
    assert(n.isNormalized());
    float tanT = cross(v, n).norm() / dot(v, n);
    float res;
    if (dot(v, m) / dot(v, n) < 0.f) res = 0.f;
    else res = 2.f / (1.f + sqrt(1.f + alpha * alpha * tanT * tanT));
    assert(res >= 0.f);
    assert(res <= 1.f);
    return res;
}

vec3 MetalMaterial::eval(vec3 wi, vec3 wo, vec3 n)
{
    // maybe need to use sign()...
    vec3 hr = (wi + wo).normalized();

    // return GGXF(wi, hr) * GGXG(wi, hr, n, alpha) * GGXG(wo, hr, n, alpha) * GGXD(hr, n, alpha) * 0.25f / (dot(wi, n) * dot(wo, n)) * albedo;
    // return GGXD(wo, hr, n, alpha) * 0.25f / (dot(wi, n) * dot(wo, n)) * albedo;
    return GGXD(hr, n, alpha) * albedo;
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
