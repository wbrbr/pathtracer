#include "camera.hpp"
#include "util.hpp"

Camera::Camera(glm::vec3 position, glm::vec3 target): position(position), direction(glm::normalize(target - position))
{
}

Ray Camera::getRay(int x, int y, int width, int height)
{
    glm::vec3 right = glm::cross(direction, glm::vec3(0.f, 1.f, 0.f));
    glm::vec3 up = glm::cross(right, direction);
    // std::cout << "Right: " << right << std::endl;
    // std::cout << "Up: " << up << std::endl;
    float yf =  -(((float)y + 0.5f) * 2.f / (float)height - 1.f);
    float xf = ((float)x + 0.5f) * 2.f / (float)width - 1.f;
    float u = xf + random_between(-0.5f / (float)width, 0.5f / (float)width);
    float v = yf + random_between(-0.5f / (float)height, 0.5f / (float)height);
	glm::vec3 offset = u * right + v * up;
	return Ray(position, direction + offset);
}

