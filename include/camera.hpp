#ifndef CAMERA_HPP
#define CAMERA_HPP
#include "ray.hpp"

class Camera
{
public:
	Camera(glm::vec3 position, glm::vec3 target);
	Ray getRay(int x, int y, int width, int height);

private:
	glm::vec3 position;
	glm::vec3 direction;
};
#endif
