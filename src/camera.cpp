#include "camera.hpp"
#include "util.hpp"
#include <iostream>

Camera::Camera(glm::vec3 position, glm::vec3 target, float focal_dist): position(position), direction(glm::normalize(target - position)), focal(focal_dist)
{
}

Ray Camera::getRay(int x, int y, int width, int height)
{
    glm::vec3 right = glm::cross(direction, glm::vec3(0.f, 1.f, 0.f));
    glm::vec3 up = glm::cross(right, direction);
    float aspect = (float)width / (float)height;
    float yf =  -(((float)y + 0.5f) * 2.f / (float)height - 1.f);
    float xf = ((float)x + 0.5f) * 2.f / (float)width - 1.f;
    xf *= aspect;
    float u = xf + random_between(-0.5f / (float)width, 0.5f / (float)width); // adjusted for aspect ratio
    float v = yf + random_between(-0.5f / (float)height, 0.5f / (float)height); // [-1, 1]
	glm::vec3 offset = u * right + v * up;
	return Ray(position, glm::normalize(direction * focal + offset));
}

