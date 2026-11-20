#include "Framebuffer.h"
#include <glad/gl.h>

Texture::Texture() { glCreateTextures(GL_TEXTURE_2D, 1, &handle); }

void Texture::update(
    int width, int height, void *data, int internal_format, unsigned format
) {
    glTexImage2D(
        GL_TEXTURE_2D, 0, internal_format, width, height, 0, format,
        GL_UNSIGNED_BYTE, data
    );
}

void Texture::set_parameters(
    std::initializer_list<std::pair<unsigned, int>> parameters
) {
    for (auto [name, param] : parameters) {
        glTextureParameteri(handle, name, param);
    }
}

Texture::~Texture() { glDeleteTextures(1, &handle); }

void Framebuffer::resize(unsigned width, unsigned height) {
    color_texture.update(
        static_cast<int>(width), static_cast<int>(height), nullptr, GL_RGB,
        GL_RGB
    );
    depth_stencil.update(
        static_cast<int>(width), static_cast<int>(height), nullptr,
        GL_DEPTH24_STENCIL8, GL_UNSIGNED_INT_24_8
    );
}

Framebuffer::Framebuffer() {

    glCreateFramebuffers(1, &framebuffer);
    glNamedFramebufferTexture(
        framebuffer, GL_COLOR_ATTACHMENT0, color_texture.get(), 0
    );
    glNamedFramebufferTexture(
        framebuffer, GL_DEPTH_STENCIL_ATTACHMENT, depth_stencil.get(), 0
    );
}

void Framebuffer::use() { glBindFramebuffer(GL_FRAMEBUFFER, framebuffer); }

Framebuffer::~Framebuffer() {
    if (framebuffer) {
        glDeleteFramebuffers(1, &framebuffer);
    }
}
