#ifndef TRIANGLE_MESH_HPP
#define TRIANGLE_MESH_HPP
#include <optional>
#include <vector>
#include <string>
#include "shape.hpp"
#include "ray.hpp"
#include "glm/vec3.hpp"

class TriangleMesh;

class Triangle
{
public:
    Box boundingBox();
	std::optional<IntersectionData> intersects(Ray ray);
    float area();
    glm::vec3 normal();

	TriangleMesh* tm;
	int i0, i1, i2;
    Material* mat;
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

    ~BVHNode();
    std::optional<IntersectionData> intersects(Ray ray);
    
};

class TriangleMesh: public Shape
{
public:
	//TriangleMesh( Material* material, std::vector<Triangle>& triangles, std::vector<glm::vec3>& vertices);
    TriangleMesh();
	std::optional<IntersectionData> intersects(Ray ray);
	glm::vec3 getVertex(int i);

	std::vector<glm::vec3> vertices;
    BVHNode root;
};

class World;
class Material;
void loadObj(std::string path, World* world, Material* mat);
#endif
