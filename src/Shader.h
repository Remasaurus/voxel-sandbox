#include <filesystem>
#include <string_view>

class Shader {
public:
    Shader(Shader&&);
    Shader operator=(Shader&&);

    void add_stage(const std::filesystem::path& path, unsigned shader_type);

    void set_uniform(std::string_view name, int value);
    void link();
    void use();

    ~Shader();

    static constexpr size_t info_log_size = 512;

private:
    void delete_program();

    unsigned program = 0;
};
