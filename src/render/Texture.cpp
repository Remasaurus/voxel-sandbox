#include "Texture.h"
#include <glad/gl.h>
#include <cassert>
#include <limits>

Texture::Texture(unsigned width, unsigned height, unsigned internal_format)
    : width_(width), height_(height) {

    assert(width <= std::numeric_limits<int>::max());
    assert(height <= std::numeric_limits<int>::max());

    glCreateTextures(GL_TEXTURE_2D, 1, &texture);
    glTextureStorage2D(
        texture, 1, internal_format, 
        static_cast<int>(width), static_cast<int>(height)
    );
}

Texture::~Texture() { delete_texture(); }

Texture::Texture(Texture && rhs) {
    texture = std::exchange(rhs.texture, 0);
    width_ = std::exchange(rhs.width_, 0);
    height_ = std::exchange(rhs.height_, 0);
}

Texture& Texture::operator=(Texture && rhs) {
    delete_texture();
    texture = std::exchange(rhs.texture, 0);
    width_ = std::exchange(rhs.width_, 0);
    height_ = std::exchange(rhs.height_, 0);

    return *this;
}

void Texture::delete_texture() {
    if (texture) {
        glDeleteTextures(1, &texture);
    }
}

void Texture::write(
    unsigned format, void *data
) { 
    write(0, 0, width_, height_, format, data);
}

void Texture::write(
    unsigned width, unsigned height,
    unsigned format, void *data
) { 
    write(0, 0, width, height, format, data);
}

void Texture::write(
    unsigned x_offset, unsigned y_offset, unsigned width, unsigned height,
    unsigned format, void *data
) {
    assert(x_offset <= std::numeric_limits<int>::max());
    assert(y_offset <= std::numeric_limits<int>::max());
    assert(width <= std::numeric_limits<int>::max());
    assert(height <= std::numeric_limits<int>::max());

    assert(x_offset + width <= width_);
    assert(y_offset + height <= height_);

    glTextureSubImage2D(
        texture, 0,
        static_cast<int>(x_offset), static_cast<int>(y_offset),
        static_cast<int>(width), static_cast<int>(height),
        format, GL_UNSIGNED_BYTE, data
    );
}

void Texture::generate_mipmap() { 
    glGenerateTextureMipmap(texture);
}

void Texture::bind_texture_unit(unsigned unit) {
    assert(unit < 16 && "Texture unit is only guaranteed to have 16 units");
    glBindTextureUnit(unit, texture);
}

void Texture::bind_image_texture(unsigned unit, unsigned access, unsigned format) {
    assert(unit < 16 && "Texture unit is only guaranteed to have 16 units");
    glBindImageTexture(
        unit, texture, 0, GL_FALSE, 0,
        access, format
    );
}

void Texture::set_parameters(
    std::initializer_list<std::pair<unsigned, int>> parameters
) {
    for (auto [name, param] : parameters) {
        glTextureParameteri(texture, name, param);
    }
}


