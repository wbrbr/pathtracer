#ifndef IMAGETEXTURE_HPP
#define IMAGETEXTURE_HPP
#include <string>
#include <glm/vec3.hpp>

class ImageTexture
{
public:
    ImageTexture(std::string path);
    ~ImageTexture();
    glm::vec3 get(float u, float v);

private:
    unsigned int w, h;
    glm::vec3* data;
};
#endif
