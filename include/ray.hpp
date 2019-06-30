#ifndef RAY_HPP
#define RAY_HPP
#include "vec3.hpp"

class Ray
{
public:
    vec3 o; // origin
    vec3 d; // direction

    Ray(vec3 o, vec3 d);
    static Ray from_to(vec3 o, vec3 target);
    vec3 at(float t);
};

#endif
