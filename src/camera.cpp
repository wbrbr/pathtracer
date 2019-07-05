#include "camera.hpp"
#include "util.hpp"

Camera::Camera(vec3 position, vec3 target): position(position), direction((target - position).normalized())
{
}

Ray Camera::getRay()
{
	vec3 rnd = random_unit_disk();
	std::pair<vec3, vec3> onb = plane_onb_from_normal(direction);
	vec3 offset = rnd.x * 0.5f * onb.first + rnd.y * 0.5f * onb.second;
	return Ray(position, direction + offset);
}

