#ifndef UTIL_HPP
#define UTIL_HPP
#include "vec3.hpp"
#include <utility>

vec3 random_unit_sphere();
vec3 random_unit_hemisphere(vec3 normal);
vec3 random_unit_disk(); // (x, y, 0)
float random_between(float a, float b);

std::pair<vec3, vec3> plane_onb_from_normal(vec3 n);
#endif
