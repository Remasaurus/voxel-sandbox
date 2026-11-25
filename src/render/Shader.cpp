#include "Shader.h"
#include "../Log.h"
#include <fstream>
#include <glad/gl.h>
#include <glm/gtc/type_ptr.hpp>



Shader create_compute_shader(const std::filesystem::path& comp_path) {
    Shader shader;
    shader.add_stage(comp_path, GL_COMPUTE_SHADER);
    shader.link();
    return shader;
}

Shader create_pixel_shader(const std::filesystem::path& vert_path, const std::filesystem::path &frag_path) {
    Shader shader;
    shader.add_stage(vert_path, GL_VERTEX_SHADER);
    shader.add_stage(frag_path, GL_FRAGMENT_SHADER);
    shader.link();
    return shader;
}

void Shader::set_uniform(const std::string& name, int value) {
	glUniform1i(glGetUniformLocation(program, name.c_str()), value);
}
void Shader::set_uniform(const std::string& name, unsigned value) {
	glUniform1ui(glGetUniformLocation(program, name.c_str()), value);
}
void Shader::set_uniform(const std::string& name, float value) {
	glUniform1f(glGetUniformLocation(program, name.c_str()), value);
}
void Shader::set_uniform(const std::string& name, const glm::ivec2& value) {
	glUniform2i(glGetUniformLocation(program, name.c_str()), value.x, value.y);
}
void Shader::set_uniform(const std::string& name, const glm::vec2& value) {
	glUniform2f(glGetUniformLocation(program, name.c_str()), value.x, value.y);
}
void Shader::set_uniform(const std::string& name, const glm::vec3& value) {
	glUniform3f(glGetUniformLocation(program, name.c_str()), value.x, value.y, value.z);
}

void Shader::set_uniform(const std::string& name, const glm::mat4& value) {
	glUniformMatrix4fv(glGetUniformLocation(program, name.c_str()), 1, false, glm::value_ptr(value));
}

namespace {
std::string read_file(const std::filesystem::path &path) {
    std::ifstream file(path.string());
    if (!file.is_open()) {
        Log::error("Could not open {}", path.string());
        return "";
    }

    const uintmax_t file_size = std::filesystem::file_size(path);
    std::string file_buffer(file_size, '\0');
    file.read(
        reinterpret_cast<char *>(file_buffer.data()),
        static_cast<long>(file_size)
    );

    return file_buffer;
}
} // namespace

void Shader::use() {
    if (!program) {
        Log::error("Tried using an undefined shader");
        return;
    }
    glUseProgram(program);
}

void Shader::add_stage(
    const std::filesystem::path &path, unsigned shader_type
) {
    std::string code = read_file(path);

    Log::info("Compiling {}", path.c_str());
    unsigned shader = glCreateShader(shader_type);

    // Cannot take address of r-value,
    // so we need to copy it to some variable.
    const char *code_c_str = code.c_str();
    glShaderSource(shader, 1, &code_c_str, NULL);
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char info_log[info_log_size];
        glGetShaderInfoLog(shader, info_log_size, NULL, info_log);
        Log::error("{}", info_log);
    }

    if (!program) {
        program = glCreateProgram();
    }

    glAttachShader(program, shader);
    glDeleteShader(shader);
}

void Shader::link() {
    if (!program) {
        Log::error("Tried linking an undefined shader");
        return;
    }

    glLinkProgram(program);

    int success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char info_log[info_log_size];

        glGetProgramInfoLog(program, info_log_size, NULL, info_log);
        Log::error("{}", info_log);
    }
}

void Shader::delete_program() {
    if (!program) {
        glDeleteProgram(program);
    }
}

Shader::~Shader() { delete_program(); }

Shader::Shader(Shader &&other) {
    delete_program();
    program = other.program;
    other.program = 0;
}

Shader &Shader::operator=(Shader &&other) {
    delete_program();
    program = other.program;
    other.program = 0;
    return *this;
}
