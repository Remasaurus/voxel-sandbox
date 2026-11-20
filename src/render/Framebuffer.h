#include <filesystem>
#include <initializer_list>
#include <utility>

class Texture {
public:
    Texture();

    void set_parameters(
        std::initializer_list<std::pair<unsigned, int>> parameters
    );

    void use(int unit = 0);

    void allocate(
        int width, int height, int internal_format, unsigned format,
        void *data = nullptr
    );
    void write(
        int x_offset, int y_offset, int width, int height, unsigned format,
        void *data
    );

    void generate_mipmap();

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

Texture load_image(const std::filesystem::path &path);
