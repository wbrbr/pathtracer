#ifndef TRIANGLE_MESH_HPP
#define TRIANGLE_MESH_HPP
#include <optional>
#include <vector>
#include "shape.hpp"
#include "vec3.hpp"
#include "ray.hpp"

class TriangleMesh;

class Triangle
{
public:
	Triangle(TriangleMesh* tm, int i0, int i1, int i2);
    Box boundingBox();
	std::optional<IntersectionData> intersects(Ray ray);

private:
	TriangleMesh* tm;
	int i0, i1, i2;
};

struct BVHNode
{
    std::vector<Triangle> triangles;
    Box box;
    BVHNode* left;
    BVHNode* right;
    // TODO: destructor
    // TODO: union (leaf / node)
    // TODO: triangles, not Shape*
    //
    std::optional<IntersectionData> intersects(Ray ray);
};

class TriangleMesh: public Shape
{
public:
	TriangleMesh(std::string path, Material* material);
	std::optional<IntersectionData> intersects(Ray ray);
	vec3 getVertex(int i);
    Material* getMaterial();

private:
	Material* material;
	std::vector<Triangle> triangles;
	std::vector<vec3> vertices;
    BVHNode root;
};
#endif
