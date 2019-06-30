#ifndef WORLD_HPP
#define WORLD_HPP
#include "sphere.hpp"
#include <vector>

class World
{
public:
    World();
    void add(Sphere s);
    std::optional<IntersectionData> intersects(Ray ray);

private:
    std::vector<Sphere> spheres;
};
#endif
