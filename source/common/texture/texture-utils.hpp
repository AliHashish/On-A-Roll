#pragma once

#include "texture2d.hpp"
#include <string>

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/vec2.hpp>

    typedef glm::vec<4, glm::uint8, glm::defaultp> ColorTex;

namespace our::texture_utils {

    // This function create an empty texture with a specific format (useful for framebuffers)
    Texture2D* empty(GLenum format, glm::ivec2 size);
    // This function loads an image and sends its data to the given Texture2D 
    Texture2D* loadImage(const std::string& filename, bool generate_mipmap = true);

    void singleColor(GLuint texture, ColorTex color={255,255,255,255}, glm::ivec2 size={1,1});
}