#ifndef PBRT_PARSER_HPP
#define PBRT_PARSER_HPP
#include "world.hpp"
#include "camera.hpp"
#include <utility>

std::pair<World, Camera> parsePbrt(std::string path);

#endif // PBRT_PARSER_HPP
