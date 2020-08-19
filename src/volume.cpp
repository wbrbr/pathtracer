#include "volume.hpp"
#include "util.hpp"
#include <tuple>

HomogeneousVolume::HomogeneousVolume(float g, float sigma_t, float sigma_s): g(g), sigma_t(sigma_t), sigma_s(sigma_s)
{}

float HomogeneousVolume::sampleDistance()
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
