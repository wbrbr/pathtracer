#include "box.hpp"
#include <cmath>

Box::Box(): min(glm::vec3(0.f, 0.f, 0.f)), max(glm::vec3(0.f, 0.f, 0.f))
{
}

Box::Box(glm::vec3 min, glm::vec3 max): min(min), max(max)
{
    center = 0.5f * (min + max);
}

glm::vec3 Box::getMin()
{
    return min;
}

glm::vec3 Box::getMax()
{
    return max;
}

glm::vec3 Box::getCenter()
{
    return center;
}

Box Box::add(Box box)
{
    glm::vec3 Min(fmin(min.x, box.min.x), fmin(min.y, box.min.y), fmin(min.z, box.min.z));
    glm::vec3 Max(fmax(max.x, box.max.x), fmax(max.y, box.max.y), fmax(max.z, box.max.z));
    return Box(Min, Max);
}

bool Box::intersects(Ray r)
{
    for (int a = 0; a < 3; a++)
    {
        float invD = 1.f / r.d[a];
        float t0 = (min[a] - r.o[a]) * invD;
        float t1 = (max[a] - r.o[a]) * invD;
        if (invD < 0.f) std::swap(t0, t1);
        float tmin = t0 > 0.f ? t0 : 0.f;
        if (t1 <= tmin) {
            return false;
        }
    }
    return true;
}
