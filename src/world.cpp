#include "world.hpp"
#include "util.hpp"
#include "material.hpp"
#include <algorithm>
#include <iostream>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/norm.hpp>
#include <optional>

World::World()
{
    envlight = nullptr;
}

void World::add(Shape* s)
{
    shapes.push_back(s);
}

void World::addLight(Triangle* tri)
{
    lights.push_back(tri);
}

std::optional<IntersectionData> World::intersects(Ray ray)
{
    std::vector<IntersectionData> inters;

    for (Shape* s : shapes) 
    {
        auto inter = s->intersects(ray);
        if (inter) inters.push_back(*inter);
    }

    if (inters.size() == 0) return {};

    IntersectionData closest = inters[0];

    for (IntersectionData inter : inters)
    {
        if (inter.t < closest.t) closest = inter;
    }

    return closest;
}

std::pair<glm::vec3, float> World::sampleLights(glm::vec3 p)
{
    assert(lights.size() > 0);
    int light_id = random_int(0, lights.size()-1);

    glm::vec3 point = uniformSampleTriangle(*lights[light_id]);
    glm::vec3 v = point - p;
    glm::vec3 dir = glm::normalize(v);

    float area = lights[light_id]->area();
    float cos = fabs(glm::dot(lights[light_id]->normal(), -dir));
    float pdf = glm::length2(v) / ((float)lights.size() * area * cos);
    return std::make_pair(point, pdf);
}

glm::vec3 World::directLighting(Ray ray, IntersectionData inter)
{
    unsigned int n = lights.size();
    if (envlight != nullptr) n++;
    if (n == 0) return glm::vec3(0.);
    int light_id = random_int(0, n-1);

    glm::vec3 Ld(0.);

    // the function that we want to integrate
    auto fn = [inter, ray, light_id, this](glm::vec3 dir, glm::vec3 emitted) { return std::abs(glm::dot(dir, inter.normal)) * inter.material->eval(-ray.d, dir, inter.normal) * emitted; };

    // LIGHT SAMPLING
    if ((unsigned)light_id == lights.size()) { // Sample environment lighting
        glm::vec3 dir;
        float p;
        std::tie(dir, p) = envlight->sampleDirection(inter.normal);

        Ray light_ray(ray.at(inter.t) + 0.0001f * inter.normal, dir);
        auto light_inter = intersects(light_ray);
        if (!light_inter) {
            PDF* pdf_brdf = inter.material->getPDF(inter.normal);
            glm::vec3 f = fn(dir, envlight->emitted(dir));
            // std::cout << f.x << " : " << p << std::endl;
            Ld += f / p;
            delete pdf_brdf;
        }
    } else {
        glm::vec3 point = uniformSampleTriangle(*lights[light_id]);
        glm::vec3 v = point - ray.at(inter.t);
        glm::vec3 dir = glm::normalize(v);

        Ray light_ray(ray.at(inter.t) + 0.0001f * inter.normal, dir);
        auto light_inter = intersects(light_ray);

        if (light_inter && light_inter->t >= glm::length(v) - 0.001 && glm::dot(lights[light_id]->normal(), -dir) > 0) {
            float area = lights[light_id]->area();
            float cos = fabs(glm::dot(lights[light_id]->normal(), -dir));
            float p = glm::length2(v) / (area * cos);

            PDF* pdf_brdf = inter.material->getPDF(inter.normal);
            Ld += fn(dir, lights[light_id]->mat->emitted()) / (p + pdf_brdf->value(dir));
            delete pdf_brdf;
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

