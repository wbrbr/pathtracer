#include "camera.hpp"
#include "util.hpp"
#include <iostream>

Camera::Camera(glm::vec3 position, glm::vec3 target, float fov)
    : position(position)
    , direction(glm::normalize(target - position))
    , fov(fov)
{
}

Ray Camera::getRay(int x, int y, int width, int height)
{
    assert(width > 0);
    assert(height > 0);
    assert(x >= 0 && x < width);
    assert(y >= 0 && y < height);
    float d = 1.f / std::tan(.5f * fov);

    glm::vec3 right = glm::cross(direction, glm::vec3(0.f, 1.f, 0.f));
    glm::vec3 up = glm::cross(right, direction);
    float aspect = (float)width / (float)height;
    float yf = -(((float)y + 0.5f) * 2.f / (float)height - 1.f); // [-1, 1]
    float xf = ((float)x + 0.5f) * 2.f / (float)width - 1.f; // [-1. 1]
    assert(xf > -1 && xf < 1);
    assert(yf > -1 && yf < 1);
    // xf *= aspect;
    float u = xf + random_between(-0.5f / (float)width, 0.5f / (float)width);
    float v = yf + random_between(-0.5f / (float)height, 0.5f / (float)height);
    glm::vec3 offset = u * right + v * up;
    glm::vec3 dir = u * aspect * right + v * up + d * direction;
    return Ray(position, dir);
}
