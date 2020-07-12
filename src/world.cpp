#include "world.hpp"
#include "util.hpp"
#include <algorithm>
#include <iostream>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/norm.hpp>

World::World()
{
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
    // TODO: handle occlusion
    float pdf = glm::length2(v) / ((float)lights.size() * area * cos);
    return std::make_pair(point, pdf);
}

float World::lightPdf(Ray ray)
{
    float pdf = 0;
    
    for (Triangle* tri : lights)
    {
        // TODO: handle occlusion
        auto inter = tri->intersects(ray);
        if (!inter) continue;

        float area = tri->area();
        float cos = fabs(glm::dot(tri->normal(), -ray.d));
        pdf += inter->t * inter->t / (area * cos);
    }
    pdf /= (float)lights.size();

    return pdf;
}
