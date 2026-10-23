#include "Application.h"
#include "Log.h"
#include <GL/glext.h>

namespace {
const char *gl_string(GLenum var) {
    return reinterpret_cast<const char *>(glGetString(var));
}

void log_gl() {
    Log::info("OpenGL vendor:       {}", gl_string(GL_VENDOR));
    Log::info("OpenGL renderer:     {}", gl_string(GL_RENDERER));
    Log::info("OpenGL version:      {}", gl_string(GL_VERSION));
    Log::info(
        "OpenGL glsl version: {}", gl_string(GL_SHADING_LANGUAGE_VERSION)
    );
}
} // namespace

int main(void) {
    glfwInit();

    // TODO: add command line argument support
    int exit_code;
    {
        Application app;

        log_gl();

        exit_code = app.run();
    }

    glfwTerminate();

    return exit_code;
}
