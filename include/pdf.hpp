#ifndef PDF_HPP
#define PDF_HPP
#include "vec3.hpp"

class PDF {
public:
    virtual ~PDF();
    virtual float value(vec3 v) = 0;
    virtual vec3 sample() = 0;
};

class UniformHemispherePDF: public PDF {
public:
    UniformHemispherePDF(vec3 n);
    float value(vec3 v);
    vec3 sample();

private:
    vec3 n;
};

class CosineHemispherePDF: public PDF {
public:
    CosineHemispherePDF(vec3 n);
    float value(vec3 v);
    vec3 sample();

private:
    vec3 n;
};
#endif
