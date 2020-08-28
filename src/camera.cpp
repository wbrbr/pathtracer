#include "camera.hpp"
#include "util.hpp"
#include <iostream>

Camera::Camera(glm::vec3 position, glm::vec3 target, float focal_dist): position(position), direction(glm::normalize(target - position)), focal(focal_dist)
{
}

Ray Camera::getRay(int x, int y, int width, int height)
{
    assert(width > 0);
    assert(height > 0);
    assert(x >= 0 && x < width);
    assert(y >= 0 && y < height);
    glm::vec3 right = glm::cross(direction, glm::vec3(0.f, 1.f, 0.f));
    // std::cout << 1 << std::endl;
    glm::vec3 up = glm::cross(right, direction);
    // std::cout << 2 << std::endl;
    float aspect = (float)width / (float)height;
    // std::cout << 3 << std::endl;
    float yf =  -(((float)y + 0.5f) * 2.f / (float)height - 1.f);
    // std::cout << 4 << std::endl;
    float xf = ((float)x + 0.5f) * 2.f / (float)width - 1.f;
    // std::cout << 5 << std::endl;
    xf *= aspect;
    // std::cout << 6 << std::endl;
    float u = xf + random_between(-0.5f / (float)width, 0.5f / (float)width); // adjusted for aspect ratio
    // std::cout << 7 << std::endl;
    float v = yf + random_between(-0.5f / (float)height, 0.5f / (float)height); // [-1, 1]
    // std::cout << 9 << std::endl;
	glm::vec3 offset = u * right + v * up;
    // std::cout << 10 << std::endl;
	return Ray(position, glm::normalize(direction * focal + offset));
}

