#include "intersector.hpp"
#include <iostream>
#include "trianglemesh.hpp"

void BVHIntersector::add(std::unique_ptr<Shape> shape)
{
    shapes.push_back(std::move(shape));
}

void BVHIntersector::build() {
}

std::optional<IntersectionData> BVHIntersector::intersects(Ray ray)
{
    std::vector<IntersectionData> inters;

    for (auto& s : shapes) {
        auto inter = s->intersects(ray);
        if (inter)
            inters.push_back(*inter);
    }

    if (inters.size() == 0)
        return {};

    IntersectionData closest = inters[0];

    for (const IntersectionData& inter : inters) {
        if (inter.t < closest.t)
            closest = inter;
    }

    return closest;
}

void embree_error(void* userPtr, enum RTCError error, const char* str)
{
    (void)userPtr;
    std::cerr << "error " << error << ": " << str << std::endl;
}

EmbreeIntersector::EmbreeIntersector()
{
    device = rtcNewDevice(NULL);
    if (!device) {
        std::cerr << "Error " << rtcGetDeviceError(device) << ": could not create Embree device" << std::endl;
    }

    rtcSetDeviceErrorFunction(device, embree_error, NULL);

    scene = rtcNewScene(device);
}

EmbreeIntersector::~EmbreeIntersector()
{
    rtcReleaseScene(scene);
    rtcReleaseDevice(device);
}

void EmbreeIntersector::add(std::unique_ptr<Shape> shape)
{
    TriangleMesh* tm = dynamic_cast<TriangleMesh*>(shape.get());
    assert(tm);
    RTCGeometry geom = rtcNewGeometry(device, RTC_GEOMETRY_TYPE_TRIANGLE);
    float* vertices = (float*)rtcSetNewGeometryBuffer(geom,RTC_BUFFER_TYPE_VERTEX,0,RTC_FORMAT_FLOAT3,3*sizeof(float),tm->vertices.size());
    rtcSetGeometryVertexAttributeCount(geom,1);
    float* normals = (float*)rtcSetNewGeometryBuffer(geom,RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE,0,RTC_FORMAT_FLOAT3,3*sizeof(float),tm->normals.size());
    unsigned int* indices = (unsigned int*)rtcSetNewGeometryBuffer(geom, RTC_BUFFER_TYPE_INDEX,0,RTC_FORMAT_UINT3,3*sizeof(unsigned int), tm->indices.size());

    for (unsigned int i = 0; i < tm->vertices.size(); i++) {
        vertices[3*i+0] = tm->vertices[i].x;
        vertices[3*i+1] = tm->vertices[i].y;
        vertices[3*i+2] = tm->vertices[i].z;
    }

    for (unsigned int i = 0; i < tm->normals.size(); i++) {
        normals[3*i+0] = tm->normals[i].x;
        normals[3*i+1] = tm->normals[i].y;
        normals[3*i+2] = tm->normals[i].z;
    }

    for (unsigned int i = 0; i < tm->indices.size(); i++) {
        indices[3*i+0] = tm->indices[i].x;
        indices[3*i+1] = tm->indices[i].y;
        indices[3*i+2] = tm->indices[i].z;
    }

    rtcSetGeometryUserData(geom, (void*)(size_t)tm->material);

    rtcCommitGeometry(geom);
    rtcAttachGeometry(scene,geom);
    rtcReleaseGeometry(geom);
}

void EmbreeIntersector::build()
{
    rtcCommitScene(scene);
}

std::optional<IntersectionData> EmbreeIntersector::intersects(Ray ray)
{
    struct RTCIntersectContext context;
    rtcInitIntersectContext(&context);

    struct RTCRayHit rayhit;
    rayhit.ray.org_x = ray.o.x;
    rayhit.ray.org_y = ray.o.y;
    rayhit.ray.org_z = ray.o.z;
    rayhit.ray.dir_x = ray.d.x;
    rayhit.ray.dir_y = ray.d.y;
    rayhit.ray.dir_z = ray.d.z;
    rayhit.ray.tnear = 0;
    rayhit.ray.tfar = INFINITY;
    rayhit.ray.mask = -1;
    rayhit.ray.flags = 0;
    rayhit.hit.geomID = RTC_INVALID_GEOMETRY_ID;
    rayhit.hit.instID[0] = RTC_INVALID_GEOMETRY_ID;

    rtcIntersect1(scene, &context, &rayhit);

    if (rayhit.hit.geomID != RTC_INVALID_GEOMETRY_ID) {
        IntersectionData inter;
        inter.t = rayhit.ray.tfar;
        inter.p = ray.at(inter.t);
        RTCGeometry geom = rtcGetGeometry(scene, rayhit.hit.geomID);
        inter.material = (MaterialID)(size_t)rtcGetGeometryUserData(geom);

        float buf[4];
        struct RTCInterpolateArguments interp;
        interp.geometry = geom;
        interp.primID = rayhit.hit.primID;
        interp.u = rayhit.hit.u;
        interp.v = rayhit.hit.v;
        interp.bufferType = RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE;
        interp.bufferSlot = 0;
        interp.P = buf;
        interp.dPdu = NULL;
        interp.dPdv = NULL;
        interp.ddPdudu = NULL;
        interp.ddPdvdv = NULL;
        interp.ddPdudv = NULL;
        interp.valueCount = 3;
        rtcInterpolate(&interp);

        inter.normal = glm::normalize(glm::vec3(buf[0],buf[1],buf[2]));
        //inter.normal = glm::normalize(glm::vec3(rayhit.hit.Ng_x, rayhit.hit.Ng_y, rayhit.hit.Ng_z));

        return inter;
    } else {
        return {};
    }
}
