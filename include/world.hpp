#ifndef WORLD_HPP
#define WORLD_HPP
#include "shape.hpp"
#include <vector>

class World: public Shape
{
public:
    World();
    void add(Shape* s);
    std::optional<IntersectionData> intersects(Ray ray);

private:
	// TODO: use smart pointers instead ? (unique_ptr) or delete in destructor
    std::vector<Shape*> shapes;
};
#endif
