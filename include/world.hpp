#ifndef WORLD_HPP
#define WORLD_HPP
#include "box.hpp"
#include "light.hpp"
#include "shape.hpp"
#include "trianglemesh.hpp"
#include "id_types.hpp"
#include <memory>
#include <utility>
#include <vector>

class World {
public:
    World();

    ShapeID add(std::unique_ptr<Shape> shape);
    void addLight(Triangle tri);
    MaterialID addMaterial(std::unique_ptr<Material> material);
    std::optional<IntersectionData> intersects(Ray ray);
    std::pair<glm::vec3, float> sampleLights(glm::vec3 p);
    glm::vec3 directLighting(Ray ray, IntersectionData inter);
    float lightPdf(Ray ray);

    Shape* getShape(ShapeID id);
    Material* getMaterial(MaterialID id);

    std::unique_ptr<EnvLight> envlight;

private:
    std::vector<std::unique_ptr<Shape>> shapes;
    std::vector<std::unique_ptr<Material>> materials;

    // make this an index into the shapes array
    std::vector<Triangle> lights;
};
#endif
