#include <initializer_list>
#include <utility>
#include <glm/glm.hpp>

class Texture {
public:
    Texture() = default;
    Texture(unsigned width, unsigned height, unsigned internal_format);
    Texture(Texture &&);
    Texture(const Texture &) = delete;
    Texture &operator=(Texture &&);
    Texture &operator=(const Texture &) = delete;

    void bind_texture_unit(unsigned unit = 0);
    void bind_image_texture(unsigned unit, unsigned access, unsigned format);

    void write(
        unsigned format, void *data
    );
    void write(
        unsigned width, unsigned height,
        unsigned format, void *data
    );
    void write(
        unsigned x_offset, unsigned y_offset, unsigned width, unsigned height,
        unsigned format, void *data
    );

    void set_parameters(
        std::initializer_list<std::pair<unsigned, int>> parameters
    );

    void generate_mipmap();
    glm::uvec2 size() { return { width_, height_ }; }

    unsigned get() { return texture; };

    ~Texture();

private:
    void delete_texture();

    unsigned texture = 0;
    unsigned width_ = 0;
    unsigned height_ = 0;
};

// Texture load_image(const std::filesystem::path &path);
