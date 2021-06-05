#include "pbrt_parser.hpp"
#include "pbrtParser/Scene.h"
#include "material.hpp"
#include "sphere.hpp"
#include <glm/gtx/string_cast.hpp>

// TODO: stop the memory leaks (use unique_ptr to store the shapes and materials in world)

std::unique_ptr<Material> convertMaterial(pbrt::Material::SP in_mat) {
    // TODO: handle textures
    // TODO: don't leak memory
    if (in_mat->as<pbrt::MatteMaterial>()) {
        pbrt::MatteMaterial::SP matte = in_mat->as<pbrt::MatteMaterial>();
        return std::make_unique<DiffuseMaterial>(glm::vec3(matte->kd.x, matte->kd.y, matte->kd.z));
    } else {
        return std::make_unique<DiffuseMaterial>(glm::vec3(1.f, 0.f, 1.f));
    }
}

glm::vec3 convertVec3(pbrt::vec3f v) {
    return glm::vec3(v.x, v.y, v.z);
}

std::pair<World, Camera> parsePbrt(std::string path)
{
    pbrt::Scene::SP scene = pbrt::importPBRT(path);
    
    pbrt::Object::SP obj = scene->world;

    World world;

    for (const pbrt::Shape::SP& shape : obj->shapes) {
        if (shape->as<pbrt::TriangleMesh>()) {
            pbrt::TriangleMesh::SP in_tm = shape->as<pbrt::TriangleMesh>();
            auto tm = std::make_unique<TriangleMesh>();

            for (pbrt::vec3f v : in_tm->vertex) {
                tm->vertices.push_back(glm::vec3(v.x, v.y, v.z));
            }

            pbrt::Material::SP in_mat = in_tm->material;

            MaterialID material = -1;

            // TODO: not mutually exclusive
            if (in_tm->areaLight) {
                pbrt::DiffuseAreaLightRGB::SP diffuseAreaLight = in_tm->areaLight->as<pbrt::DiffuseAreaLightRGB>();
                if (!diffuseAreaLight) {
                    std::cerr << "blackbody area lights are not suppported" << std::endl;
                    exit(1);
                }
                material = world.addMaterial(std::make_unique<EmissionMaterial>(convertVec3(diffuseAreaLight->L)));
            } else {
                material = world.addMaterial(convertMaterial(in_mat));
            }
            

            for (pbrt::vec3i i : in_tm->index) {
                Triangle tri;
                tri.tm = tm.get();
                tri.i0 = i.x;
                tri.i1 = i.y;
                tri.i2 = i.z;
                tri.mat = material;
                tm->root.triangles.push_back(tri);
            }

            if (in_tm->areaLight) {
                for (const Triangle& triangle : tm->root.triangles) {
                    world.addLight(triangle);
                }
            }

            buildBVHNode(&tm->root);
            world.add(std::move(tm));
        }
    }

    pbrt::Camera::SP in_cam = scene->cameras[0];
    glm::vec3 cam_pos(in_cam->frame.p.x, in_cam->frame.p.y, in_cam->frame.p.z);
    pbrt::vec3f target = in_cam->frame.p + in_cam->frame.l.vz;
    glm::vec3 target_pos(target.x, target.y, target.z);

    Camera cam(cam_pos, target_pos, in_cam->fov * M_PI / 180.f);

    return std::make_pair(std::move(world), std::move(cam));
}
