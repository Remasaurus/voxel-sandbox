#include "Framebuffer.h"
#include <glad/gl.h>

Texture::Texture() { glCreateTextures(GL_TEXTURE_2D, 1, &handle); }
Texture::~Texture() { glDeleteTextures(1, &handle); }

void Texture::allocate(
    int width, int height, int internal_format, unsigned format, void *data
) {
    glTexImage2D(
        GL_TEXTURE_2D, 0, internal_format, width, height, 0, format,
        GL_UNSIGNED_BYTE, data
    );
}

void Texture::write(
    int x_offset, int y_offset, int width, int height, unsigned format,
    void *data
) {
    glTexSubImage2D(
        GL_TEXTURE_2D, 0, x_offset, y_offset, width, height, format,
        GL_UNSIGNED_BYTE, data
    );
}

void Texture::generate_mipmap() { glGenerateMipmap(GL_TEXTURE_2D); }

void Texture::use(int unit) { (void)unit; }

void Texture::set_parameters(
    std::initializer_list<std::pair<unsigned, int>> parameters
) {
    for (auto [name, param] : parameters) {
        glTextureParameteri(handle, name, param);
    }
}

void Framebuffer::resize(unsigned width, unsigned height) {
    (void)width;
    (void)height;
}

Framebuffer::Framebuffer() {
    glCreateFramebuffers(1, &framebuffer);
    // glNamedFramebufferTexture(
    //     framebuffer, GL_COLOR_ATTACHMENT0, color_texture.get(), 0
    // );
    // glNamedFramebufferTexture(
    //     framebuffer, GL_DEPTH_STENCIL_ATTACHMENT, depth_stencil.get(), 0
    // );
}

void Framebuffer::use() { glBindFramebuffer(GL_FRAMEBUFFER, framebuffer); }

Framebuffer::~Framebuffer() {
    if (framebuffer) {
        glDeleteFramebuffers(1, &framebuffer);
    }
}
