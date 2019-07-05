#ifndef SHAPE_HPP
#define SHAPE_HPP
#include <optional>
#include "ray.hpp"

class Material;

struct IntersectionData
{
	float t;
	vec3 normal;
	Material* material;
};

class Shape
{
public:
	virtual std::optional<IntersectionData> intersects(Ray ray) = 0;
};

#endif

