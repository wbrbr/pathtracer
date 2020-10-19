#include "material.hpp"
#include "util.hpp"
#include "plot.hpp"
#include <cmath>
#include <iostream>
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
    return new CosineHemispherePDF(n);
    // return new UniformHemispherePDF(n);
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
    return new CosineHemispherePDF(n);
    // return new UniformHemispherePDF(n);
}

glm::vec3 GGXF(glm::vec3 wi, glm::vec3 hr, glm::vec3 F0)
{
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
    float ndotv = dot(n, v);
    if (ndotv < 0) return 0.;

    float num = 2.f * ndotv;
    float a2 = alpha * alpha;

    float denum = ndotv + sqrt(a2 + (1.f - a2) * ndotv * ndotv);
    float val = num / denum;
    return val;
}

float GGXG(glm::vec3 v, glm::vec3 l, glm::vec3 n, float alpha)
{
    return G1(n, v, alpha) * G1(n, l, alpha);
}

glm::vec3 MetalMaterial::eval(glm::vec3 wi, glm::vec3 wo, glm::vec3 n)
{
    glm::vec3 hr = glm::normalize(wi + wo);

    assert(dot(wi, n) >= 0.);
    assert(dot(wo, n) >= 0.);
    glm::vec3 F0 = albedo;
    glm::vec3 F = GGXF(wi, hr, F0);
    float G = GGXG(wi, wo, n, alpha);
    float D = GGXD(hr, n, alpha);
    return F * G * D * .25f / (dot(wi, n) * dot(wo, n)) * albedo;
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
