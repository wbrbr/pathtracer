#include "world.hpp"
#include "material.hpp"
#include "util.hpp"
#include <algorithm>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include <optional>

World::World()
{
    envlight = nullptr;
}

MaterialID World::add(std::unique_ptr<Shape> s)
{
    shapes.push_back(std::move(s));
    return (int)(shapes.size() - 1);
}

void World::addLight(std::unique_ptr<Light> light)
{
    lights.push_back(std::move(light));
}

MaterialID World::addMaterial(std::unique_ptr<Material> material)
{
    materials.push_back(std::move(material));
    return (int)(materials.size() - 1);
}

std::optional<IntersectionData> World::intersects(Ray ray)
{
    std::vector<IntersectionData> inters;

    for (auto& s : shapes) {
        auto inter = s->intersects(ray);
        if (inter)
            inters.push_back(*inter);
    }

    if (inters.size() == 0)
        return {};

    IntersectionData closest = inters[0];

    for (const IntersectionData& inter : inters) {
        if (inter.t < closest.t)
            closest = inter;
    }

    return closest;
}

#if 0
std::pair<glm::vec3, float> World::sampleLights(glm::vec3 p)
{
    assert(lights.size() > 0);
    int light_id = random_int(0, lights.size() - 1);

    glm::vec3 point = uniformSampleTriangle(lights[light_id]);
    glm::vec3 v = point - p;
    glm::vec3 dir = glm::normalize(v);

    float area = lights[light_id].area();
    float cos = fabs(glm::dot(lights[light_id].normal(), -dir));
    float pdf = glm::length2(v) / ((float)lights.size() * area * cos);
    return std::make_pair(point, pdf);
}
#endif

bool World::isOccluded(glm::vec3 from, glm::vec3 to, Light* light)
{
    Ray shadow_ray = Ray::from_to(from, to);
    auto inter = intersects(shadow_ray);

    if (!inter)
        return false;

    if (light->isDirectionalLight()) {
        return true;
    } else {
        return inter->t * inter->t < glm::distance2(from, to);
    }
}

glm::vec3 World::directLighting(Ray ray, IntersectionData inter)
{
    unsigned int n = lights.size();
    if (envlight != nullptr)
        n++;
    if (n == 0)
        return glm::vec3(0.);
    int light_id = random_int(0, n - 1);

    glm::vec3 Ld(0.);

    Material* mat = getMaterial(inter.material);
    // the function that we want to integrate
    auto fn = [inter, ray, light_id, mat, this](glm::vec3 dir, glm::vec3 emitted) { return std::abs(glm::dot(dir, inter.normal)) * mat->eval(-ray.d, dir, inter.normal) * emitted; };

    // LIGHT SAMPLING
    if ((unsigned)light_id == lights.size()) { // Sample environment lighting
        glm::vec3 dir;
        float p;
        std::tie(dir, p) = envlight->sampleDirection(inter.normal);

        Ray light_ray(ray.at(inter.t) + 0.0001f * inter.normal, dir);
        auto light_inter = intersects(light_ray);
        if (!light_inter) {
            glm::vec3 f = fn(dir, envlight->emitted(dir));
            Ld += f / p;
        }
    } else {
        /* glm::vec3 point = uniformSampleTriangle(lights[light_id]);
        glm::vec3 v = point - ray.at(inter.t);
        glm::vec3 dir = glm::normalize(v);

        Ray light_ray(ray.at(inter.t) + 0.0001f * inter.normal, dir);
        auto light_inter = intersects(light_ray);

        if (light_inter && light_inter->t >= glm::length(v) - 0.001 && glm::dot(lights[light_id].normal(), -dir) > 0) {
            float area = lights[light_id].area();
            float cos = fabs(glm::dot(lights[light_id].normal(), -dir));
            float p = glm::length2(v) / (area * cos);

            PDF* pdf_brdf = mat->getPDF(inter.normal);
            Ld += fn(dir, getMaterial(lights[light_id].mat)->emitted()) / (p + pdf_brdf->value(dir));
            delete pdf_brdf;
        } */
        glm::vec3 from = ray.at(inter.t);

        float p;
        glm::vec3 emitted;
        glm::vec3 lightSample = lights[light_id]->samplePosition(inter, p, emitted);
        glm::vec3 dir = lightSample - from;

        if (glm::dot(inter.normal, dir) >= 0 && !isOccluded(from + 0.0001f * inter.normal, lightSample, lights[light_id].get())) {
            Ld += fn(dir, emitted) / p;
        }
    }

    // BRDF SAMPLING
    /* PDF* pdf = inter.material->getPDF(inter.normal);
    glm::vec3 dir = pdf->sample(); 
    float p = pdf->value(dir);
    glm::vec3 brdf = inter.material->eval(-ray.d, dir, inter.normal);

    Ray light_ray(ray.o + inter.t * ray.d + 0.0001f * inter.normal, dir);
    
    // visibility check
    if ((unsigned)light_id == lights.size()) {
        auto scene_inter = intersects(light_ray);
        // float light_pdf = glm::dot(light_ray.d, inter.normal) * M_1_PI;
        if (!scene_inter) Ld += fn(dir, envlight->emitted(dir)) / (p + light_pdf);
    } else {
        auto light_inter = lights[light_id]->intersects(light_ray);
        if (light_inter && glm::dot(lights[light_id]->normal(), -dir) > 0) {
            auto scene_inter = intersects(light_ray);
            if (light_inter->t >= scene_inter->t - 0.001f) {
                float area = lights[light_id]->area();
                float cos = std::abs(glm::dot(lights[light_id]->normal(), -dir));
                float light_pdf = light_inter->t * light_inter->t / (area * cos);

                Ld += fn(dir, lights[light_id]->mat->emitted()) / (p + light_pdf);
            }
        }
    }
    delete pdf; */

    return Ld * (float)n;
}

Material* World::getMaterial(MaterialID id)
{
    return materials[id].get();
}