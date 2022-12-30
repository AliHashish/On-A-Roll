#include "texture-utils.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include <iostream>


our::Texture2D* our::texture_utils::empty(GLenum format, glm::ivec2 size){
    our::Texture2D* texture = new our::Texture2D();
    //TODO: (Req 11) Finish this function to create an empty texture with the given size and format [DONE]

    texture->bind();
    //Set Unpack Alignment to 4-byte (it means that each row takes multiple of 4 bytes in memory)
    //Note: this is not necessary since:
    //- Alignment is 4 by default
    //- Alignment of 1 or 2 will still work correctly but 8 may cause problems
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    //Send data to texture
    //NOTE: the internal format is set to GL_RGBA8 so every pixel contains 4 bytes, one for each channel
    // glTexStorage2D(GL_TEXTURE_2D, 0, format, size.x, size.y);
    glTexImage2D(GL_TEXTURE_2D, 0, format, size.x, size.y, 0, format, GL_UNSIGNED_BYTE, nullptr);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    
    texture->unbind();
    return texture;
}

our::Texture2D* our::texture_utils::loadImage(const std::string& filename, bool generate_mipmap) {

    glm::ivec2 size;
    int channels;
    //Since OpenGL puts the texture origin at the bottom left while images typically has the origin at the top left,
    //We need to till stb to flip images vertically after loading them
    stbi_set_flip_vertically_on_load(true);
    //Load image data and retrieve width, height and number of channels in the image
    //The last argument is the number of channels we want and it can have the following values:
    //- 0: Keep number of channels the same as in the image file
    //- 1: Grayscale only
    //- 2: Grayscale and Alpha
    //- 3: RGB
    //- 4: RGB and Alpha (RGBA)
    //Note: channels (the 4th argument) always returns the original number of channels in the file
    unsigned char* pixels = stbi_load(filename.c_str(), &size.x, &size.y, &channels, 4);
    if(pixels == nullptr){
        std::cerr << "Failed to load image: " << filename << std::endl;
        return nullptr;
    }
    // Create a texture
    our::Texture2D* texture = new our::Texture2D();
    //Bind the texture such that we upload the image data to its storage
    //TODO: (Req 5) Finish this function to fill the texture with the data found in "pixels"    [DONE]
    texture->bind();
    //Set Unpack Alignment to 4-byte (it means that each row takes multiple of 4 bytes in memory)
    //Note: this is not necessary since:
    //- Alignment is 4 by default
    //- Alignment of 1 or 2 will still work correctly but 8 may cause problems
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    //Send data to texture
    //NOTE: the internal format is set to GL_RGBA8 so every pixel contains 4 bytes, one for each channel
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, size.x, size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    //Generate versions of the texture at smaller level of details (useful for filtering)
    if(generate_mipmap) glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(pixels); //Free image data after uploading to GPU
    texture->unbind();
    return texture;
}

our::Texture2D* our::texture_utils::singleColor(ColorTex color, glm::ivec2 size){
    our::Texture2D* texture = new our::Texture2D();
    //Bind the texture such that we upload the image data to its storage
    //TODO: (Req 5) Finish this function to fill the texture with the data found in "pixels"    [DONE]
    texture->bind();
    //Allocate array for texture data
    auto* data = new ColorTex[size.x * size.y];
    //Fill array with the same color
    std::fill_n(data, size.x * size.y, color);
    //Bind the texture such that we upload the image data to its storage
    //Set Unpack Alignment to 4-byte (it means that each row takes multiple of 4 bytes in memory)
    //Note: this is not necessary since:
    //- Alignment is 4 by default
    //- Alignment of 1 or 2 will still work correctly but 8 will cause problems
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    //Send data to texture
    //NOTE: the internal format is set to GL_RGBA8 so every pixel contains 4 bytes, one for each channel
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, size.x, size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    //Generate Mipmaps after loading the texture
    glGenerateMipmap(GL_TEXTURE_2D);
    delete[] data;
    return texture;
}