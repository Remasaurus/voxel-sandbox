#include <filesystem>
#include <string_view>
#include <glm/glm.hpp>


class Shader {
public:
    Shader() = default;
    Shader(Shader &&);
    Shader(const Shader &) = delete;
    Shader &operator=(Shader &&);
    Shader &operator=(const Shader &) = delete;

    void add_stage(const std::filesystem::path &path, unsigned shader_type);

    void link();
    void use();

	void set_uniform(const std::string& name, int value);
	void set_uniform(const std::string& name, float value);
	void set_uniform(const std::string& name, const glm::ivec2& value);
	void set_uniform(const std::string& name, const glm::vec2& value);
	void set_uniform(const std::string& name, const glm::vec3& value);
	void set_uniform(const std::string& name, const glm::mat4& value);

    ~Shader();

    static constexpr size_t info_log_size = 512;

private:
    void delete_program();

    unsigned program = 0;
};

Shader create_compute_shader(const std::filesystem::path& comp_path);
Shader create_pixel_shader(const std::filesystem::path& vert_path, const std::filesystem::path &frag_path);
