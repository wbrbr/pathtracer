#ifndef INTERSECTOR_HPP
#define INTERSECTOR_HPP
#include <memory>
#include <optional>
#include <vector>
#include "shape.hpp"
#include "embree3/rtcore.h"

class Intersector {
public:
    virtual ~Intersector() = default;

    virtual void add(std::unique_ptr<Shape> shape) = 0;
    virtual void build() = 0;
    virtual std::optional<IntersectionData> intersects(Ray ray) = 0;
};

class BVHIntersector: public Intersector {
public:
    virtual void add(std::unique_ptr<Shape> shape) override;
    virtual void build() override;
    virtual std::optional<IntersectionData> intersects(Ray ray) override;

private:
    std::vector<std::unique_ptr<Shape>> shapes;
};

class EmbreeIntersector: public Intersector {
public:
    EmbreeIntersector();
    ~EmbreeIntersector();
    virtual void add(std::unique_ptr<Shape> shape) override;
    virtual void build() override;
    virtual std::optional<IntersectionData> intersects(Ray ray) override;

private:
    RTCDevice device;
    RTCScene scene;
};

#endif // INTERSECTOR_HPP
