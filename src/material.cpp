#include "material.hpp"
#include "util.hpp"
#include "plot.hpp"
#include <cmath>
#include <cassert>

DiffuseMaterial::DiffuseMaterial(vec3 albedo): albedo(albedo)
{
}

PDF* DiffuseMaterial::getPDF(vec3 n)
{
    // return new CosineHemispherePDF(n);
    return new UniformHemispherePDF(n);
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
    //return new CosineHemispherePDF(n);
    return new UniformHemispherePDF(n);
}

float GGXF(vec3 wi, vec3 hr)
{
    const float F0 = 0.04;
    float x = 1.f - dot(hr, wi);
    return F0 + (1.f - F0) * x * x * x * x * x;
}

// http://graphicrants.blogspot.com/2013/08/specular-brdf-reference.html
float GGXD(vec3 hr, vec3 n, float alpha) 
{
    float ndoth = dot(n, hr);
    if (ndoth < 0.f) return 0.f;

    float a2 = alpha*alpha;
    float denum = ndoth * ndoth * (a2 - 1.f) + 1.f;
    return a2 / (M_PI * denum * denum);
}

float G1(vec3 n, vec3 v, float alpha)
{
    assert(v.isNormalized());
    assert(n.isNormalized());
    float ndotv = dot(n, v);

    float num = 2.f * ndotv;
    float a2 = alpha * alpha;

    float denum = ndotv + sqrt(a2 + (1.f - a2) * ndotv * ndotv);
    return num / denum;
}

float GGXG(vec3 v, vec3 l, vec3 n, float alpha)
{
    return G1(n, v, alpha) * G1(n, l, alpha);
}

vec3 MetalMaterial::eval(vec3 wi, vec3 wo, vec3 n)
{
    // maybe need to use sign()...
    vec3 hr = (wi + wo).normalized();

    return GGXD(hr, n, alpha) * albedo;
    // return GGXD(hr, n, alpha) * 0.25f / (dot(wi, n) * dot(wo, n)) * albedo;
    return GGXF(wi, hr) * GGXG(wi, wo, n, alpha) * GGXD(hr, n, alpha) * 0.25f / (dot(wi, n) * dot(wo, n)) * albedo;
    // return GGXD(wo, hr, n, alpha) * 0.25f / (dot(wi, n) * dot(wo, n)) * albedo;
    /* #pragma once
    plot("ggxd.data", [](float t){ return GGXD(t, 0.04); }, 0.f, 0.2f, 0.001f); */

    //return GGXD(dot(n, hr), alpha) * albedo;
    //return dot(n, hr) * albedo;
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
