#include "pbrtParser/Scene.h"
#include "pbrt_parser.hpp"

std::pair<World, Camera> parsePbrt(std::string path) {
    pbrt::Scene::SP scene = pbrt::importPBRT(path);

    World world;
    Camera camera(glm::vec3(0,0,0), glm::vec3(1,1,1), 50);

    pbrt::Object::SP obj = scene->world;

    for (const pbrt::Shape::SP& shape : obj->shapes) {
        if (shape->as<pbrt::TriangleMesh>()) {
            pbrt::TriangleMesh::SP in_tm = shape->as<pbrt::TriangleMesh>();

            TriangleMesh tm;

            for (pbrt::vec3f v : in_tm->vertex) {
                tm.vertices.push_back(glm::vec3(v.x,v.y,v.z));
            }
        }
    }
    return std::make_pair(std::move(world), std::move(camera));
}
