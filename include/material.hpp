#ifndef MATERIAL_HPP
#define MATERIAL_HPP
#include "pdf.hpp"
#include "ray.hpp"
#include "shape.hpp"
#include <optional>

struct ScatterData {
    glm::vec3 attenuation;
    std::optional<Ray> scattered;
};

// TODO: material = BSDF + emission
class Material {
public:
    // virtual ScatterData scatter(Ray ray, IntersectionData inter) = 0;
    virtual PDF* getPDF(glm::vec3 n) = 0;
    virtual glm::vec3 eval(glm::vec3 wi, glm::vec3 wo, glm::vec3 n) = 0;
    virtual glm::vec3 emitted();
};

class DiffuseMaterial : public Material {
public:
    DiffuseMaterial(glm::vec3 albedo);
    PDF* getPDF(glm::vec3 n);
    glm::vec3 eval(glm::vec3 wi, glm::vec3 wo, glm::vec3 n);

private:
    glm::vec3 albedo;
};

class MetalMaterial : public Material {
public:
    MetalMaterial(glm::vec3 albedo, float alpha);
    PDF* getPDF(glm::vec3 n);
    glm::vec3 eval(glm::vec3 wi, glm::vec3 wo, glm::vec3 n);

private:
    glm::vec3 albedo;
    float alpha;
};

class EmissionMaterial : public Material {
public:
    EmissionMaterial(glm::vec3 emission);
    PDF* getPDF(glm::vec3 n);
    glm::vec3 eval(glm::vec3 wi, glm::vec3 wo, glm::vec3 n);
    glm::vec3 emitted();

private:
    glm::vec3 emission;
};

#endif
