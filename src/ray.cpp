#include "ray.hpp"

Ray::Ray(vec3 o, vec3 d): o(o), d(d.normalized())
{
}

vec3 Ray::at(float t)
{
    return o + t*d;
}

Ray Ray::from_to(vec3 o, vec3 target)
{
    vec3 direction = target - o;
    return Ray(o, direction);
}
