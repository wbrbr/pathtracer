#include "world.hpp"

World::World()
{
}

void World::add(Shape* s)
{
    shapes.push_back(s);
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