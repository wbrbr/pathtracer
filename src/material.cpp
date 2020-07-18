#include "material.hpp"
#include "util.hpp"
#include "plot.hpp"
#include <cmath>
#include <cassert>

glm::vec3 Material::emitted()
{
    return glm::vec3(0.f, 0.f, 0.f);
}

DiffuseMaterial::DiffuseMaterial(glm::vec3 albedo): albedo(albedo)
{
}

PDF* DiffuseMaterial::getPDF(glm::vec3 n)
{
    // return new CosineHemispherePDF(n);
    return new UniformHemispherePDF(n);
}

// Color
glm::vec3 DiffuseMaterial::eval(glm::vec3 wi, glm::vec3 wo, glm::vec3 n)
{
    return (float)M_1_PI * albedo;
}

MetalMaterial::MetalMaterial(glm::vec3 albedo, float alpha): albedo(albedo), alpha(alpha)
{
}

PDF* MetalMaterial::getPDF(glm::vec3 n)
{
    // return new CosineHemispherePDF(n);
    return new UniformHemispherePDF(n);
}

float GGXF(glm::vec3 wi, glm::vec3 hr)
{
    const float F0 = 0.04;
    float x = 1.f - glm::dot(hr, wi);
    return F0 + (1.f - F0) * x * x * x * x * x;
}

// http://graphicrants.blogspot.com/2013/08/specular-brdf-reference.html
float GGXD(glm::vec3 hr, glm::vec3 n, float alpha) 
{
    float ndoth = glm::dot(n, hr);
    if (ndoth < 0.f) return 0.f;

    float a2 = alpha*alpha;
    float denum = ndoth * ndoth * (a2 - 1.f) + 1.f;
    return a2 / (M_PI * denum * denum);
}

float G1(glm::vec3 n, glm::vec3 v, float alpha)
{
    /* assert(v.isNormalized());
    assert(n.isNormalized()); */
    float ndotv = dot(n, v);

    float num = 2.f * ndotv;
    float a2 = alpha * alpha;

    float denum = ndotv + sqrt(a2 + (1.f - a2) * ndotv * ndotv);
    return num / denum;
}

float GGXG(glm::vec3 v, glm::vec3 l, glm::vec3 n, float alpha)
{
    return G1(n, v, alpha) * G1(n, l, alpha);
}

glm::vec3 MetalMaterial::eval(glm::vec3 wi, glm::vec3 wo, glm::vec3 n)
{
    // maybe need to use sign()...
    glm::vec3 hr = glm::normalize(wi + wo);

    //return GGXD(hr, n, alpha) * albedo;
    // return GGXD(hr, n, alpha) * 0.25f / (dot(wi, n) * dot(wo, n)) * albedo;
    return GGXF(wi, hr) * GGXG(wi, wo, n, alpha) * GGXD(hr, n, alpha) * 0.25f / (dot(wi, n) * dot(wo, n)) * albedo;
    // return GGXD(wo, hr, n, alpha) * 0.25f / (dot(wi, n) * dot(wo, n)) * albedo;
    /* #pragma once
    plot("ggxd.data", [](float t){ return GGXD(t, 0.04); }, 0.f, 0.2f, 0.001f); */

    //return GGXD(dot(n, hr), alpha) * albedo;
    //return dot(n, hr) * albedo;
}

EmissionMaterial::EmissionMaterial(glm::vec3 emission)
{
    this->emission = emission;
}

PDF* EmissionMaterial::getPDF(glm::vec3 n)
{
    return new UniformHemispherePDF(n);
}

glm::vec3 EmissionMaterial::eval(glm::vec3 wi, glm::vec3 wo, glm::vec3 n)
{
    return glm::vec3(0.f, 0.f, 0.f);
}

glm::vec3 EmissionMaterial::emitted()
{
    return emission;
}
