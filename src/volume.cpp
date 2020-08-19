#include "volume.hpp"
#include "util.hpp"
#include <tuple>
#include <openvdb/tools/Interpolation.h>

HomogeneousVolume::HomogeneousVolume(float g, float sigma_t, float albedo): g(g), sigma_t(sigma_t), albedo(albedo)
{}

float HomogeneousVolume::sampleDistance(Ray ray)
{
    return -log(1 - random_between(0., 1.)) / sigma_t;
}

glm::vec3 henyey_greenstein(float g, glm::vec3 dir)
{
    // Henyey-Greenstein from PRRT
    float cosTheta;

    if (std::abs(g) < 1e-3) {
        cosTheta = 1 - 2 * random_between(0., 1.);
    } else {
        float sqr = (1 - g * g) / (1 - g + 2 * g * random_between(0., 1.));
        cosTheta = (1 + g*g - sqr * sqr) / (2 * g);
    }

    float sinTheta = std::sqrt(std::max(0.f, 1-cosTheta*cosTheta));
    float phi = random_between(0.f, 2.f * M_PI);
    
    glm::vec3 v1, v2;
    std::tie(v1, v2) = plane_onb_from_normal(dir);

    return sinTheta * std::cos(phi) * v1 + sinTheta * std::sin(phi) * v2 + cosTheta * dir;
}

glm::vec3 HomogeneousVolume::samplePhase(glm::vec3 dir)
{
    return henyey_greenstein(g, dir);
}

HeterogeneousVolume::HeterogeneousVolume(float g, std::string vdb_path, float albedo): g(g), albedo(albedo)
{
    openvdb::io::File file(vdb_path);
    file.open();
    openvdb::GridBase::Ptr baseGrid = file.readGrid("density");
    file.close();
    grid = openvdb::gridPtrCast<openvdb::FloatGrid>(baseGrid);

    sigma_hat = 0.;

    for (auto iter = grid->cbeginValueAll(); iter.test(); ++iter)
    {
        float v = *iter;
        sigma_hat = std::max(sigma_hat, v);
    }
    // Request a value accessor for accelerated access.
    // // (Because value accessors employ a cache, it is important to declare
    // // one accessor per thread.)
    // GridType::ConstAccessor accessor = grid.getConstAccessor();
    // // Instantiate the GridSampler template on the accessor type and on
    // // a box sampler for accelerated trilinear interpolation.
    // openvdb::tools::GridSampler<GridType::ConstAccessor, openvdb::tools::BoxSampler>
    //     fastSampler(accessor, grid.transform());
}

float HeterogeneousVolume::sampleDistance(Ray ray)
{
    openvdb::tools::GridSampler<openvdb::FloatGrid, openvdb::tools::BoxSampler> sampler(*grid);

    float t = 0;
    for (;;) {
        t -= log(1 - random_between(0., 1.)) / sigma_hat;

        glm::vec3 pos = ray.at(t);
        float sigma_t = sampler.wsSample(openvdb::Vec3R(pos.x, pos.y, pos.z));

        // get the density
        if (random_between(0., 1.) < sigma_t / sigma_hat) {
            break;
        }
    }

    return t;
}

glm::vec3 HeterogeneousVolume::samplePhase(glm::vec3 dir)
{
    return henyey_greenstein(g, dir);
}
