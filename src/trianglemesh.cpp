#include "trianglemesh.hpp"
#include <cassert>
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

Triangle::Triangle(TriangleMesh* tm, int i0, int i1, int i2) : tm(tm), i0(i0), i1(i1), i2(i2)
{
}

std::optional<IntersectionData> Triangle::intersects(Ray ray)
{
	const float EPSILON = 0.0000001;
	vec3 vertex0 = tm->getVertex(i0);;
	vec3 vertex1 = tm->getVertex(i1);
	vec3 vertex2 = tm->getVertex(i2);
	vec3 edge1, edge2, h, s, q;
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
		inter.normal = cross(edge1, edge2); // TODO: cache triangle normal
		return inter;
	}
	else // This means that there is a line intersection but not a ray intersection.
		return {};
}

TriangleMesh::TriangleMesh(std::string path, Material* material): material(material)
{
	/* vertices.push_back(vec3(-0.5f, -0.5f, 0.5f));
	vertices.push_back(vec3(0.5f, -0.5f, 0.5f));
	vertices.push_back(vec3(-0.5f, 0.5f, 0.5f));
	vertices.push_back(vec3(0.5f, 0.5f, 0.5f));
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

	for (int i = 0; i < attrib.vertices.size() / 3; i++)
	{
		vertices.push_back(vec3(attrib.vertices[3 * i], attrib.vertices[3 * i + 1], attrib.vertices[3 * i + 2]));
	}

	assert(shapes.size() > 0);
	tinyobj::mesh_t mesh = shapes[0].mesh;
	for (int i = 0; i < mesh.num_face_vertices.size(); i++)
	{
		assert(mesh.num_face_vertices[i] == 3);
		triangles.push_back(Triangle(this, mesh.indices[3 * i].vertex_index, mesh.indices[3 * i + 1].vertex_index, mesh.indices[3 * i + 2].vertex_index));
	}
}

std::optional<IntersectionData> TriangleMesh::intersects(Ray ray)
{
	IntersectionData closest;
	closest.t = INFINITY;
	for (Triangle t : triangles)
	{
		auto inter = t.intersects(ray);
		if (inter && inter->t < closest.t) {
			closest = *inter;
		}
	}
	closest.material = material;
	return (closest.t < INFINITY) ? closest : std::optional<IntersectionData>{};
}

vec3 TriangleMesh::getVertex(int i)
{
	assert(i < vertices.size());
	return vertices.at(i);
}