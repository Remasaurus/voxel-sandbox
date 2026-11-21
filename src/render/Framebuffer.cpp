#include "Framebuffer.h"
#include <glad/gl.h>
#include <cassert>
#include "../Log.h"

Framebuffer::~Framebuffer() { delete_framebuffer(); }
Framebuffer::Framebuffer(Framebuffer && rhs) {
    framebuffer = std::exchange(rhs.framebuffer, 0);
}
Framebuffer& Framebuffer::operator=(Framebuffer && rhs) {
    delete_framebuffer();
    framebuffer = std::exchange(rhs.framebuffer, 0);

    return *this;
}

Framebuffer::Framebuffer() {
    glCreateFramebuffers(1, &framebuffer);
}

Framebuffer::Framebuffer(unsigned width, unsigned height) {
    glCreateFramebuffers(1, &framebuffer);
    resize(width, height);
}

void Framebuffer::resize(unsigned width, unsigned height) {
    assert(width < std::numeric_limits<int>::max());
    assert(height < std::numeric_limits<int>::max());

    width_ = width;
    height_ = height;

    // color
    color_texture = Texture(width, height, GL_RGBA32F);
    color_texture.set_parameters({
        { GL_TEXTURE_WRAP_S, GL_REPEAT },
        { GL_TEXTURE_WRAP_T, GL_REPEAT },
        { GL_TEXTURE_MIN_FILTER, GL_LINEAR },
        { GL_TEXTURE_MAG_FILTER, GL_LINEAR },
    });
    glNamedFramebufferTexture(
        framebuffer, GL_COLOR_ATTACHMENT0, color_texture.get(), 0
    );

    // depth stencil
   glCreateRenderbuffers(1, &depth_stencil);
   glNamedRenderbufferStorage(
       depth_stencil, GL_DEPTH24_STENCIL8,
       static_cast<int>(width), static_cast<int>(height)
   );
   glNamedFramebufferRenderbuffer(
       framebuffer, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depth_stencil
   );

   auto result = glCheckNamedFramebufferStatus(framebuffer, GL_FRAMEBUFFER);
   if (result != GL_FRAMEBUFFER_COMPLETE) {
        Log::error("Failed to create framebuffer [{}]", result);
	}
}

void Framebuffer::delete_framebuffer() {
    if (framebuffer) {
        glDeleteFramebuffers(1, &framebuffer);
    }
}

