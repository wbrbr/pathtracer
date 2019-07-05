#ifndef CAMERA_HPP
#define CAMERA_HPP
#include "ray.hpp"

class Camera
{
public:
	Camera(vec3 position, vec3 target);
	Ray getRay(int x, int y, int width, int height);

private:
	vec3 position;
	vec3 direction;
};
#endif
