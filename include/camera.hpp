#ifndef CAMERA_HPP
#define CAMERA_HPP
#include "ray.hpp"

class Camera
{
public:
	Camera(vec3 position, vec3 target);
	Ray getRay();

private:
	vec3 position;
	vec3 direction;
};
#endif