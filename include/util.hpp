#ifndef UTIL_HPP
#define UTIL_HPP
#include "glm/glm.hpp"
#include "trianglemesh.hpp"
#include <utility>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#define M_1_PI 0.318309886183790671538
#endif

glm::vec3 random_unit_sphere();
glm::vec3 random_unit_hemisphere(glm::vec3 normal);
glm::vec3 random_unit_disk(); // (x, y, 0)
glm::vec3 cosine_unit_hemisphere(glm::vec3 normal);
float random_between(float a, float b);

std::pair<glm::vec3, glm::vec3> plane_onb_from_normal(glm::vec3 n);

glm::vec3 uniformSampleTriangle(Triangle& triangle);

// inclusive
int random_int(int a, int b);
#endif
