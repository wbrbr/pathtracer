#ifndef WORLD_HPP
#define WORLD_HPP
#include <vector>
#include <utility>
#include "shape.hpp"
#include "box.hpp"
#include "trianglemesh.hpp"

class World
{
public:
    World();
    void add(Shape* s);
    void addLight(Triangle* tri);
    std::optional<IntersectionData> intersects(Ray ray);
    std::pair<glm::vec3, float> sampleLights(glm::vec3 p);

private:
	// TODO: use smart pointers instead ? (unique_ptr) or delete in destructor
    std::vector<Shape*> shapes;
    std::vector<Triangle*> lights;
};
#endif
