cmake_minimum_required(VERSION 3.5)

project(pathtracer)

set(CMAKE_BUILD_TYPE RelWithDebInfo)

add_executable(pathtracer src/box.cpp src/camera.cpp src/imagetexture.cpp src/intersector.cpp src/light.cpp src/main.cpp src/material.cpp src/pbrt_parser.cpp src/pdf.cpp src/ray.cpp src/shape.cpp src/sphere.cpp src/trianglemesh.cpp src/util.cpp src/world.cpp)

target_include_directories(pathtracer PUBLIC ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/external/include)
set_property(TARGET pathtracer PROPERTY CXX_STANDARD 17)

find_package(OpenMP)

add_subdirectory(${CMAKE_SOURCE_DIR}/external/pbrt-parser)

set(embree_DIR "" CACHE PATH "Path to Embree")
find_package(embree 3.0 REQUIRED)

target_link_libraries(pathtracer PUBLIC OpenMP::OpenMP_CXX pbrtParser embree)
