#ifndef SHAPE_HPP
#define SHAPE_HPP
#include <optional>
#include "ray.hpp"
#include "box.hpp"

class Material;

struct IntersectionData
{
	float t;
	vec3 normal;
	Material* material;

    IntersectionData()
    {
        material = nullptr;
    }
};

class Shape
{
public:
    // virtual Box boundingBox() = 0;
	virtual std::optional<IntersectionData> intersects(Ray ray) = 0;
};

#endif

