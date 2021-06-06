#include "pbrt_parser.hpp"
#include "intersector.hpp"
#include "material.hpp"
#include "pbrtParser/Scene.h"
#include "sphere.hpp"
#include <glm/gtx/string_cast.hpp>

// TODO: stop the memory leaks (use unique_ptr to store the shapes and materials in world)

std::unique_ptr<Material> convertMaterial(pbrt::Material::SP in_mat)
{
    // TODO: handle textures
    if (in_mat->as<pbrt::MatteMaterial>()) {
        pbrt::MatteMaterial::SP matte = in_mat->as<pbrt::MatteMaterial>();
        return std::make_unique<DiffuseMaterial>(glm::vec3(matte->kd.x, matte->kd.y, matte->kd.z));
    } else {
        return std::make_unique<DiffuseMaterial>(glm::vec3(1.f, 0.f, 1.f));
    }
}

glm::vec3 convertVec3(pbrt::vec3f v)
{
    return glm::vec3(v.x, v.y, v.z);
}

std::pair<World, Camera> parsePbrt(std::string path)
{
    pbrt::Scene::SP scene = pbrt::importPBRT(path);

    pbrt::Object::SP obj = scene->world;

    World world;

    //auto intersector = std::make_unique<BVHIntersector>();
    auto intersector = std::make_unique<EmbreeIntersector>();

    for (const pbrt::Shape::SP& shape : obj->shapes) {
        if (shape->as<pbrt::TriangleMesh>()) {
            pbrt::TriangleMesh::SP in_tm = shape->as<pbrt::TriangleMesh>();
            auto tm = std::make_unique<TriangleMesh>();

            tm->vertices.reserve(in_tm->vertex.size());
            for (pbrt::vec3f v : in_tm->vertex) {
                tm->vertices.push_back(convertVec3(v));
            }

            tm->normals.reserve(in_tm->normal.size());
            for (pbrt::vec3f n : in_tm->normal) {
                tm->normals.push_back(convertVec3(n));
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

            tm->material = material;

            for (pbrt::vec3i i : in_tm->index) {
                Triangle tri;
                tri.tm = tm.get();
                tri.i0 = i.x;
                tri.i1 = i.y;
                tri.i2 = i.z;
                tri.mat = material;
                tm->root.triangles.push_back(tri);

                tm->indices.push_back(glm::uvec3(i.x, i.y, i.z));
            }

            /* if (in_tm->areaLight) {
                for (const Triangle& triangle : tm->root.triangles) {
                    world.addLight(triangle);
                }
            } */

            std::cout << "Building BVH (" << tm->root.triangles.size() << " tris)" << std::endl;
            buildBVHNode(&tm->root);
            intersector->add(std::move(tm));
        }
    }

    for (const pbrt::LightSource::SP& light : obj->lightSources) {
        if (light->as<pbrt::PointLightSource>()) {
            pbrt::PointLightSource::SP in_point_light = light->as<pbrt::PointLightSource>();
            world.addLight(std::make_unique<PointLight>(convertVec3(in_point_light->from), convertVec3(in_point_light->I)));
        } else if (light->as<pbrt::DistantLightSource>()) {
            pbrt::DistantLightSource::SP in_distant_light = light->as<pbrt::DistantLightSource>();
            world.addLight(std::make_unique<DirectionalLight>(glm::normalize(convertVec3(in_distant_light->to - in_distant_light->from)), convertVec3(in_distant_light->L)));
        }
    }

    intersector->build();
    world.setIntersector(std::move(intersector));
    //world.envlight = std::make_unique<ConstantEnvLight>(glm::vec3(.5f, .6f, .7f));

    pbrt::Camera::SP in_cam = scene->cameras[0];
    glm::vec3 cam_pos(in_cam->frame.p.x, in_cam->frame.p.y, in_cam->frame.p.z);

    Camera cam(cam_pos, convertVec3(in_cam->frame.l.vx), convertVec3(in_cam->frame.l.vy), convertVec3(in_cam->frame.l.vz), in_cam->fov * M_PI / 180.f);

    return std::make_pair(std::move(world), std::move(cam));
}
