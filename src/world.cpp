#include "world.hpp"
#include "util.hpp"
#include <algorithm>

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

std::pair<glm::vec3, float> World::sampleLights()
{
    assert(lights.size() > 0);
    int light_id = random_int(0, lights.size()-1);

    glm::vec3 point = uniformSampleTriangle(*lights[light_id]);
    return std::make_pair(point, 1.f / (lights[light_id]->area() * (float)lights.size()));
}
