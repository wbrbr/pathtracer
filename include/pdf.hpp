#ifndef PDF_HPP
#define PDF_HPP
#include "glm/vec3.hpp"

class PDF {
public:
    virtual ~PDF();
    virtual float value(glm::vec3 v) = 0;
    virtual glm::vec3 sample() = 0;
};

class UniformHemispherePDF: public PDF {
public:
    UniformHemispherePDF(glm::vec3 n);
    float value(glm::vec3 v);
    glm::vec3 sample();

private:
    glm::vec3 n;
};

class CosineHemispherePDF: public PDF {
public:
    CosineHemispherePDF(glm::vec3 n);
    float value(glm::vec3 v);
    glm::vec3 sample();

private:
    glm::vec3 n;
};
#endif
