#include "camera.hpp"
#include "util.hpp"

Camera::Camera(vec3 position, vec3 target): position(position), direction((target - position).normalized())
{
}

Ray Camera::getRay(int x, int y, int width, int height)
{
    vec3 right = cross(direction, vec3(0.f, 1.f, 0.f));
    vec3 up = cross(right, direction);
    // std::cout << "Right: " << right << std::endl;
    // std::cout << "Up: " << up << std::endl;
    float yf =  -(((float)y + 0.5f) * 2.f / (float)height - 1.f);
    float xf = ((float)x + 0.5f) * 2.f / (float)width - 1.f;
    float u = xf + random_between(-0.5f / (float)width, 0.5f / (float)width);
    float v = yf + random_between(-0.5f / (float)height, 0.5f / (float)height);
	vec3 offset = u * right + v * up;
	return Ray(position, direction + offset);
}

