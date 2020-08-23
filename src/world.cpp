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

float World::lightPdf(Ray ray)
{
    float pdf = 0;
    
    for (Triangle* tri : lights)
    {
        auto inter = tri->intersects(ray);
        if (!inter) continue;

        float area = tri->area();
        float cos = fabs(glm::dot(tri->normal(), -ray.d));
        pdf += inter->t * inter->t / (area * cos);
    }
    pdf /= (float)lights.size();

    return pdf;
}

glm::vec3 World::directLighting(Ray ray, IntersectionData inter)
{
    unsigned int n = lights.size();
    if (envlight != nullptr) n++;
    assert(n > 0);
    int light_id = random_int(0, n-1);

    if ((unsigned)light_id == lights.size()) { // Sample environment lighting
        glm::vec3 dir;
        float p;
        std::tie(dir, p) = envlight->sampleDirection(inter.normal);

        Ray light_ray(ray.at(inter.t) + 0.0001f * inter.normal, dir);
        auto light_inter = intersects(light_ray);
        if (light_inter) return glm::vec3(0.);

        return std::abs(glm::dot(dir, inter.normal)) * inter.material->eval(-ray.d, dir, inter.normal) * envlight->emitted(dir) * (float)n / p;
    } else {
        glm::vec3 point = uniformSampleTriangle(*lights[light_id]);
        glm::vec3 v = point - ray.at(inter.t);
        glm::vec3 dir = glm::normalize(v);

        Ray light_ray(ray.at(inter.t) + 0.0001f * inter.normal, dir);
        auto light_inter = intersects(light_ray);
        if (!light_inter || light_inter->t < glm::length(v) - 0.001 || glm::dot(lights[light_id]->normal(), -dir) <= 0) return glm::vec3(0);

        float area = lights[light_id]->area();
        float cos = fabs(glm::dot(lights[light_id]->normal(), -dir));
        float pdf = glm::length2(v) / (n * area * cos);

        return (float)fabs(glm::dot(dir, inter.normal)) * inter.material->eval(-ray.d, dir, inter.normal) * lights[light_id]->mat->emitted() / pdf;
    }

}

