#include "Shader.h"
#include "Log.h"
#include <fstream>
#include <glad/gl.h>
#include <sstream>

std::string read_file(const std::filesystem::path &path) {
    std::ifstream file;

    file.open(path.string());
    if (!file.is_open()) {
        Log::error("Could not open {}", path.string());
        return "";
    }

    std::stringstream stream;
    stream << file.rdbuf();
    return stream.str();
}

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

Shader& Shader::operator=(Shader &&other) {
    delete_program();
    program = other.program;
	other.program = 0;
	return *this;
}
