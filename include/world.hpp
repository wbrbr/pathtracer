#ifndef WORLD_HPP
#define WORLD_HPP
#include "box.hpp"
#include "id_types.hpp"
#include "intersector.hpp"
#include "light.hpp"
#include "shape.hpp"
#include "trianglemesh.hpp"
#include <memory>
#include <utility>
#include <vector>

class World {
public:
    World();

    void setIntersector(std::unique_ptr<Intersector> intersector);
    void addLight(std::unique_ptr<Light> light);
    MaterialID addMaterial(std::unique_ptr<Material> material);
    std::optional<IntersectionData> intersects(Ray ray);
    std::pair<glm::vec3, float> sampleLights(glm::vec3 p);
    glm::vec3 directLighting(Ray ray, IntersectionData inter);
    float lightPdf(Ray ray);

    Material* getMaterial(MaterialID id);

    std::unique_ptr<EnvLight> envlight;

private:
    std::vector<std::unique_ptr<Shape>> shapes;
    std::vector<std::unique_ptr<Material>> materials;
    std::unique_ptr<Intersector> intersector;

    // make this an index into the shapes array
    std::vector<std::unique_ptr<Light>> lights;

    bool isOccluded(glm::vec3 from, glm::vec3 to, Light* light);
};
#endif
