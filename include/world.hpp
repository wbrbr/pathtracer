#ifndef WORLD_HPP
#define WORLD_HPP
#include <vector>
#include <utility>
#include <memory>
#include "shape.hpp"
#include "box.hpp"
#include "trianglemesh.hpp"
#include "light.hpp"

class World
{
public:
    World();
    void add(Shape* s);
    void addLight(Triangle* tri);
    std::optional<IntersectionData> intersects(Ray ray);
    std::pair<glm::vec3, float> sampleLights(glm::vec3 p);
    glm::vec3 directLighting(Ray ray, IntersectionData inter);
    float lightPdf(Ray ray);

    std::unique_ptr<EnvLight> envlight;

private:
	// TODO: use smart pointers instead ? (unique_ptr) or delete in destructor
    std::vector<Shape*> shapes;
    std::vector<Triangle*> lights;
};
#endif
