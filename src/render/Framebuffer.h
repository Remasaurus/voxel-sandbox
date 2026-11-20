#include <initializer_list>
#include <utility>

class Texture {
public:
    Texture();

    void set_parameters(
        std::initializer_list<std::pair<unsigned, int>> parameters
    );

    unsigned get() { return handle; }
    void update(
        int width, int height, void *data, int internal_format, unsigned format
    );

    ~Texture();

private:
    unsigned handle = 0;
};

class Framebuffer {
public:
    Framebuffer();
    ~Framebuffer();

    void use();
    void resize(unsigned width, unsigned height);

private:
    unsigned framebuffer = 0;
    Texture depth_stencil;
    Texture color_texture;
};
