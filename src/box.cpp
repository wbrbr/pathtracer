#include "box.hpp"
#include <cmath>
#ifdef NANCHECK
#include <fenv.h>
#endif

Box::Box()
{
    bounds[0] = glm::vec3(0.f);
    bounds[1] = glm::vec3(0.f);
    center = glm::vec3(0.f);
}

Box::Box(glm::vec3 min, glm::vec3 max)
{
    bounds[0] = min;
    bounds[1] = max;
    center = 0.5f * (min + max);
}

glm::vec3 Box::getMin()
{
    return bounds[0];
}

glm::vec3 Box::getMax()
{
    return bounds[1];
}

glm::vec3 Box::getCenter()
{
    return center;
}

Box Box::add(Box box)
{
    glm::vec3 Min(fmin(getMin().x, box.getMin().x), fmin(getMin().y, box.getMin().y), fmin(getMin().z, box.getMin().z));
    glm::vec3 Max(fmax(getMax().x, box.getMax().x), fmax(getMax().y, box.getMax().y), fmax(getMax().z, box.getMax().z));
    return Box(Min, Max);
}

bool Box::intersects(Ray r)
{
    /* for (int a = 0; a < 3; a++)
    {
        float invD = 1.f / r.d[a];
        float t0 = (min[a] - r.o[a]) * invD;
        float t1 = (max[a] - r.o[a]) * invD;
        if (invD < 0.f) std::swap(t0, t1);
        float tmin = std::min(t0, t1);
        //float tmin = t0 > 0.f ? t0 : 0.f;
        if (t1 <= tmin) {
            return false;
        }
    }
    return true; */
    // http://www.cs.utah.edu/~awilliam/box/box.pdf
#ifdef NANCHECK
    fedisableexcept(FE_INVALID | FE_OVERFLOW);
#endif
    const float t0 = 0.f;
    const float t1 = 100.f;
    float tmin,tmax,tymin,tymax,tzmin,tzmax;
    tmin= (bounds[r.sign[0]].x - r.o.x)* r.invd.x;
    tmax= (bounds[1-r.sign[0]].x- r.o.x)* r.invd.x;
    tymin= (bounds[r.sign[1]].y- r.o.y)* r.invd.y;
    tymax= (bounds[1-r.sign[1]].y- r.o.y)* r.invd.y;
    if ( (tmin> tymax)|| (tymin> tmax)) return false;
    if (tymin> tmin) tmin= tymin;
    if (tymax< tmax) tmax= tymax;
    tzmin = (bounds[r.sign[2]].z- r.o.z)* r.invd.z;
    tzmax= (bounds[1-r.sign[2]].z- r.o.z)* r.invd.z;
    if ( (tmin> tzmax)|| (tzmin> tmax)) return false;
    if (tzmin> tmin)tmin= tzmin;
    if (tzmax< tmax) tmax= tzmax;
    return( (tmin< t1) && (tmax> t0) );
#ifdef NANCHECK
    feenableexcept(FE_INVALID | FE_OVERFLOW);
#endif
}
