#include "pbrt_parser.hpp"
#include "pbrtParser/Scene.h"
#include "material.hpp"
#include "sphere.hpp"
#include <glm/gtx/string_cast.hpp>

std::pair<World, Camera> parsePbrt(std::string path)
{
    pbrt::Scene::SP scene = pbrt::importPBRT(path);
    
    // everything becomes an instance
    //scene->makeSingleLevel();

    pbrt::Object::SP obj = scene->world;

    DiffuseMaterial* mat = new DiffuseMaterial(glm::vec3(.7f, .7f, .7f));

    World world;
    world.envlight = std::make_unique<ConstantEnvLight>(glm::vec3(1.f, 1.f, 1.f));
    for (const pbrt::Shape::SP& shape : obj->shapes) {
        if (shape->as<pbrt::TriangleMesh>()) {
            pbrt::TriangleMesh::SP in_tm = shape->as<pbrt::TriangleMesh>();

            TriangleMesh* tm = new TriangleMesh;

            for (pbrt::vec3f v : in_tm->vertex) {
                tm->vertices.push_back(glm::vec3(v.x, v.y, v.z));
            }

            for (pbrt::vec3i i : in_tm->index) {
                Triangle tri;
                tri.tm = tm;
                tri.i0 = i.x;
                tri.i1 = i.y;
                tri.i2 = i.z;
                tri.mat = mat;
                tm->root.triangles.push_back(tri);
            }
            std::cout << tm->root.triangles.size() << std::endl;
            buildBVHNode(&tm->root);
            world.add(tm);
        }
    }

    pbrt::Camera::SP in_cam = scene->cameras[0];
    glm::vec3 cam_pos(in_cam->frame.p.x, in_cam->frame.p.y, in_cam->frame.p.z);
    pbrt::vec3f target = in_cam->frame.p + in_cam->frame.l.vz;
    glm::vec3 target_pos(target.x, target.y, target.z);

    Camera cam(cam_pos, target_pos, in_cam->fov * M_PI / 180.f);

    return std::make_pair(std::move(world), std::move(cam));
}
