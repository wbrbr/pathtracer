#include "trianglemesh.hpp"
#include <cassert>
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#include <iostream>
#include <algorithm>

BVHNode::~BVHNode()
{
    if (left != nullptr) delete left; 
    if (right != nullptr) delete right;
}

std::optional<IntersectionData> BVHNode::intersects(Ray ray)
{
    if (!box.intersects(ray)) {
        return {};
    }
    if (left == nullptr) {
        IntersectionData closest;
        closest.t = INFINITY;
        for (Triangle t : triangles)
        {
            auto inter = t.intersects(ray);
            if (inter && inter->t < closest.t) {
                closest = *inter;
            }
        }

        return closest.t == INFINITY ? std::optional<IntersectionData>{} : closest;
    } else {
        assert(right != nullptr);
        auto interLeft = left->intersects(ray);
        if (interLeft) {
            auto interRight = right->intersects(ray);
            if (interRight) {
                return interRight->t < interLeft->t ? interRight : interLeft;
            } else {
                return interLeft;
            }
        }
        else return right->intersects(ray);
    }
}

bool compareX(Triangle s1, Triangle s2)
{
    return s1.boundingBox().getCenter().x < s2.boundingBox().getCenter().x;
}

bool compareY(Triangle s1, Triangle s2)
{
    return s1.boundingBox().getCenter().y < s2.boundingBox().getCenter().y;
}

bool compareZ(Triangle s1, Triangle s2)
{
    return s1.boundingBox().getCenter().z < s2.boundingBox().getCenter().z;
}

std::pair<std::vector<Triangle>, std::vector<Triangle>> splitShapes(BVHNode* node)
{
    glm::vec3 sides = node->box.getMax() - node->box.getMin();
    
    if (sides.x >= sides.y && sides.x >= sides.z) {
        std::sort(node->triangles.begin(), node->triangles.end(), compareX);
    } else if (sides.y >= sides.x && sides.y >= sides.z) {
        std::sort(node->triangles.begin(), node->triangles.end(), compareY);
    } else {
        std::sort(node->triangles.begin(), node->triangles.end(), compareZ);
    }

    auto last = node->triangles.begin() + node->triangles.size()/2;
    std::pair<std::vector<Triangle>, std::vector<Triangle>> res;
    res.first = std::vector<Triangle>(node->triangles.begin(), last);
    res.second = std::vector<Triangle>(last, node->triangles.end());

    return res;
}

Box computeBoundingBox(BVHNode* node)
{
    Box b = node->triangles.at(0).boundingBox();
    for (Triangle s : node->triangles)
    {
        b = b.add(s.boundingBox());
    }
    return b;
}

void buildBVHNode(BVHNode* node)
{
    node->box = computeBoundingBox(node);
    if (node->triangles.size() > 3) {
        auto split = splitShapes(node);
        node->left = new BVHNode;
        node->left->triangles = split.first;
        node->right = new BVHNode;
        node->right->triangles = split.second;

        buildBVHNode(node->left);
        buildBVHNode(node->right);
    } else { // leaf
        node->left = nullptr;
        node->right = nullptr;
    }
}


Triangle::Triangle(TriangleMesh* tm, int i0, int i1, int i2) : tm(tm), i0(i0), i1(i1), i2(i2)
{
}

inline float fmin(float a, float b, float c)
{
    return fmin(fmin(a, b), c);
}

inline float fmax(float a, float b, float c)
{
    return fmax(fmax(a, b), c);
}

Box Triangle::boundingBox()
{
    glm::vec3 v0 = tm->getVertex(i0);
    glm::vec3 v1 = tm->getVertex(i1);
    glm::vec3 v2 = tm->getVertex(i2);

    glm::vec3 min(fmin(v0.x, v1.x, v2.x), fmin(v0.y, v1.y, v2.y), fmin(v0.z, v1.z, v2.z));
    glm::vec3 max(fmax(v0.x, v1.x, v2.x), fmax(v0.y, v1.y, v2.y), fmax(v0.z, v1.z, v2.z));

    return Box(min, max);
}

std::optional<IntersectionData> Triangle::intersects(Ray ray)
{
	const float EPSILON = 0.0000001;
	glm::vec3 vertex0 = tm->getVertex(i0);
	glm::vec3 vertex1 = tm->getVertex(i1);
	glm::vec3 vertex2 = tm->getVertex(i2);
	glm::vec3 edge1, edge2, h, s, q;
	float a, f, u, v;
	edge1 = vertex1 - vertex0;
	edge2 = vertex2 - vertex0;
	h = cross(ray.d, edge2);
	a = dot(edge1, h);
	if (a > -EPSILON && a < EPSILON)
		return {};    // This ray is parallel to this triangle.
	f = 1.0 / a;
	s = ray.o - vertex0;
	u = f * dot(s, h);
	if (u < 0.0 || u > 1.0)
		return {};
	q = cross(s, edge1);
	v = f * dot(ray.d, q);
	if (v < 0.0 || u + v > 1.0)
		return {};
	// At this stage we can compute t to find out where the intersection point is on the line.
	float t = f * dot(edge2, q);
	if (t > EPSILON) // ray intersection
	{
		IntersectionData inter;
		inter.t = t;
		inter.normal = glm::normalize(glm::cross(edge1, edge2)); // TODO: cache triangle normal
        inter.material = tm->getMaterial();
		return inter;
	}
	else // This means that there is a line intersection but not a ray intersection.
		return {};
}

TriangleMesh::TriangleMesh(std::string path, Material* material): material(material)
{
	/* vertices.push_back(glm::vec3(-0.5f, -0.5f, 0.5f));
	vertices.push_back(glm::vec3(0.5f, -0.5f, 0.5f));
	vertices.push_back(glm::vec3(-0.5f, 0.5f, 0.5f));
	vertices.push_back(glm::vec3(0.5f, 0.5f, 0.5f));
	Triangle t1(this, 0, 1, 2);
	Triangle t2(this, 2, 1, 3);
	triangles.push_back(t1);
	triangles.push_back(t2); */
	std::vector<tinyobj::shape_t> shapes;
	tinyobj::attrib_t attrib;
	std::ifstream stream(path);
	std::string warn, err;
	tinyobj::LoadObj(&attrib, &shapes, nullptr, &warn, &err, &stream);
	if (!warn.empty()) {
		std::cerr << "Warning: " << warn << std::endl;
	}
	if (!err.empty()) {
		std::cerr << "Error: " << err << std::endl;
	}

	for (unsigned int i = 0; i < attrib.vertices.size() / 3; i++)
	{
		vertices.push_back(glm::vec3(attrib.vertices[3 * i], attrib.vertices[3 * i + 1], attrib.vertices[3 * i + 2]));
	}

	assert(shapes.size() > 0);
	tinyobj::mesh_t mesh = shapes[0].mesh;
	for (unsigned int i = 0; i < mesh.num_face_vertices.size(); i++)
	{
		assert(mesh.num_face_vertices[i] == 3);
		triangles.push_back(Triangle(this, mesh.indices[3 * i].vertex_index, mesh.indices[3 * i + 1].vertex_index, mesh.indices[3 * i + 2].vertex_index));
	}


    std::cout << "Building BVH" << std::endl;
    root.triangles = triangles;
    buildBVHNode(&root);
    std::cout << "Done" << std::endl;
}

std::optional<IntersectionData> TriangleMesh::intersects(Ray ray)
{
	/* IntersectionData closest;
	closest.t = INFINITY;
	for (Triangle t : triangles)
	{
		auto inter = t.intersects(ray);
		if (inter && inter->t < closest.t) {
			closest = *inter;
		}
	}
	closest.material = material;
	return (closest.t < INFINITY) ? closest : std::optional<IntersectionData>{}; */
    return root.intersects(ray);
}

glm::vec3 TriangleMesh::getVertex(int i)
{
	assert((unsigned int)i < vertices.size());
	return vertices.at(i);
}

Material* TriangleMesh::getMaterial()
{
    return material;
}
