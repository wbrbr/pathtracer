#ifndef PBRT_PARSER_HPP
#define PBRT_PARSER_HPP
#include "camera.hpp"
#include "world.hpp"
#include <utility>

std::pair<World, Camera> parsePbrt(std::string path);

#endif // PBRT_PARSER_HPP
