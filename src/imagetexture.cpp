#include "imagetexture.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define TINYEXR_IMPLEMENTATION
#include "tinyexr.h"
#include <iostream>
#include <filesystem>

ImageTexture::ImageTexture(std::string path)
{
    std::cout << "coucou" << std::endl;
    std::string ext = std::filesystem::path(path).extension();
    if (ext == ".hdr") {
        stbi_set_flip_vertically_on_load(1);
        int x, y, n;
        float* buf = stbi_loadf(path.c_str(), &x, &y, &n, 3);
        if (buf == nullptr) {
            std::cerr << stbi_failure_reason() << std::endl;
            exit(1);
        }
        w = (unsigned)x;
        h = (unsigned)y;
        data = reinterpret_cast<glm::vec3*>(buf);
    } else if (ext == ".exr") {
        
        const char **layer_names;
        int num_layers;
        const char* err;
        int ret = EXRLayers(path.c_str(), &layer_names, &num_layers, &err);
        if (ret != TINYEXR_SUCCESS) fprintf(stderr, "%s\n", err);
        std::cout << num_layers << std::endl;
        assert(num_layers > 0);
        float* buf;
        int x, y;
        ret = LoadEXRWithLayer(&buf, &x, &y, path.c_str(), layer_names[0], &err);
        if (ret != TINYEXR_SUCCESS) fprintf(stderr, "%s\n", err);
        w = (unsigned)x;
        h = (unsigned)y;
        data = reinterpret_cast<glm::vec3*>(buf);
    }
}

ImageTexture::~ImageTexture()
{
    // stbi_image_free(data);
    free(data);
}

glm::vec3 ImageTexture::get(float u, float v)
{
    assert(u >= 0 && u < 1 && v >= 0 && v < 1);
    u *= (float)w;
    v *= (float)h;

    unsigned xmin = (unsigned)u;
    unsigned xmax = xmin+1;
    unsigned ymin = (unsigned)v;
    unsigned ymax = ymin+1;
    if (xmax == w) xmax = xmin;
    if (ymax == h) ymax = ymin;
    float tx = u - (float)xmin;
    float ty = v - (float)ymin;
    glm::vec3 v1 = ty * data[ymax*w+xmin] + (1.f - ty) * data[ymin*w+xmin];
    glm::vec3 v2 = ty * data[ymax*w+xmax] + (1.f - ty) * data[ymin*w+xmax];
    return tx * v2 + (1.f - tx) * v1;
}

glm::vec3 ImageTexture::getPixel(unsigned int x, unsigned int y)
{
    assert(x < w && y < h);
    return data[y*w + x];
}

unsigned int ImageTexture::width()
{
    return w;
}

unsigned int ImageTexture::height()
{
    return h;
}
