#ifndef TRIANGLE_MESH_HPP
#define TRIANGLE_MESH_HPP
#include "glm/vec3.hpp"
#include "ray.hpp"
#include "shape.hpp"
#include <optional>
#include <string>
#include <vector>

class TriangleMesh;

class Triangle {
public:
    Box boundingBox();
    std::optional<IntersectionData> intersects(Ray ray);
    float area();
    glm::vec3 normal();

    TriangleMesh* tm;
    int i0, i1, i2;
    Material* mat;
};

struct BVHBuildNode {
    std::vector<Triangle> triangles;
    Box box;
    BVHBuildNode* left;
    BVHBuildNode* right;

    ~BVHBuildNode();
    std::optional<IntersectionData> intersects(Ray ray);
};

/* struct BVHNode
{
    Box box;
    BVHNode* left;
    BVHNode* right;

    std::optional<IntersectionData> intersects(Ray ray);
}; */

class TriangleMesh : public Shape {
public:
    //TriangleMesh( Material* material, std::vector<Triangle>& triangles, std::vector<glm::vec3>& vertices);
    TriangleMesh();
    void buildBVH();

    std::optional<IntersectionData> intersects(Ray ray);
    glm::vec3 getVertex(int i);

    std::vector<glm::vec3> vertices;
    BVHBuildNode root;
};

class World;
class Material;
void buildBVHNode(BVHBuildNode* node);
void loadObj(std::string path, World* world, Material* mat);
#endif
