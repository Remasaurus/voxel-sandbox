#include "Texture.h"
#include <glm/glm.hpp>

class Framebuffer {
public:
    Framebuffer();
    Framebuffer(unsigned width, unsigned height);

    Framebuffer(Framebuffer &&);
    Framebuffer(const Framebuffer &) = delete;
    Framebuffer &operator=(Framebuffer &&);
    Framebuffer &operator=(const Framebuffer &) = delete;

    void resize(unsigned width, unsigned height);

    Texture& as_texture() { return color_texture; }

    glm::uvec2 size() { return { width_, height_ }; }

    ~Framebuffer();

private:
    void delete_framebuffer();

    unsigned width_ = 0;
    unsigned height_ = 0;

    unsigned framebuffer = 0;
    unsigned depth_stencil = 0;
    Texture color_texture;
};
