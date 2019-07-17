#ifndef BOX_HPP
#define BOX_HPP
#include "vec3.hpp"
#include "ray.hpp"

class Box
{
public:
    Box();
    Box(vec3 min, vec3 max);
    vec3 getMin();
    vec3 getMax();
    vec3 getCenter();
    Box add(Box box);
    bool intersects(Ray ray);

private:
    vec3 min, max, center;
};
#endif
