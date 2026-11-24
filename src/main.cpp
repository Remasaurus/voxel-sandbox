#include "Application.h"
#include "Log.h"
#include <GL/gl.h>
#include <GL/glext.h>
#include <cstdlib>
#include <immintrin.h>
#include <xmmintrin.h>

namespace {
const char *gl_string(GLenum name) {
    return reinterpret_cast<const char *>(glGetString(name));
}
int gl_int(GLenum name) {
    int v;
    glGetIntegerv(name, &v);
    return v;
}

void log_gl() {
    Log::info("OpenGL vendor:        {}", gl_string(GL_VENDOR));
    Log::info("OpenGL renderer:      {}", gl_string(GL_RENDERER));
    Log::info("OpenGL version:       {}", gl_string(GL_VERSION));
    Log::info(
        "OpenGL glsl version:  {}", gl_string(GL_SHADING_LANGUAGE_VERSION)
    );
    Log::info(
        "OpenGL max buffer bindings: {}", gl_int(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS)
    );
}
} // namespace

int main(void) {
    glfwInit();

#ifdef NDEBUG
    Log::info("Asserts are disabled");
#else
    Log::info("Asserts are enabled");
#endif

    auto path = std::filesystem::current_path();
    std::filesystem::current_path(path.parent_path());

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
