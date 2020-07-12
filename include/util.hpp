#ifndef UTIL_HPP
#define UTIL_HPP
#include <utility>
#include "glm/glm.hpp"
#include "trianglemesh.hpp"

glm::vec3 random_unit_sphere();
glm::vec3 random_unit_hemisphere(glm::vec3 normal);
glm::vec3 random_unit_disk(); // (x, y, 0)
float random_between(float a, float b);

std::pair<glm::vec3, glm::vec3> plane_onb_from_normal(glm::vec3 n);

glm::vec3 uniformSampleTriangle(Triangle& triangle);

// inclusive
int random_int(int a, int b);
#endif
