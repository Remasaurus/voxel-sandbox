#include <glad/gl.h>
#include <imgui.h>

#include "Log.h"
#include "Renderer.h"

namespace {
void debug_message_callback(
    GLenum source, GLenum type, unsigned int id, GLenum severity,
    GLsizei length, const char *message, const void *userParam
) {
    (void)length;
    (void)userParam;

    // ignore non-significant error/warning codes
    if (id == 131169 || id == 131185 || id == 131218 || id == 131204)
        return;

    const char *source_str;

    switch (source) {
    case GL_DEBUG_SOURCE_API:
        source_str = "API";
        break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        source_str = "Window System";
        break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER:
        source_str = "Shader Compiler";
        break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:
        source_str = "Third Party";
        break;
    case GL_DEBUG_SOURCE_APPLICATION:
        source_str = "Application";
        break;
    case GL_DEBUG_SOURCE_OTHER:
        source_str = "Other";
        break;
    default:
        source_str = "Unknown";
    }

    const char *type_str;
    switch (type) {
    case GL_DEBUG_TYPE_ERROR:
        type_str = "Error";
        break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        type_str = "Deprecated Behaviour";
        break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        type_str = "Undefined Behaviour";
        break;
    case GL_DEBUG_TYPE_PORTABILITY:
        type_str = "Portability";
        break;
    case GL_DEBUG_TYPE_PERFORMANCE:
        type_str = "Performance";
        break;
    case GL_DEBUG_TYPE_MARKER:
        type_str = "Marker";
        break;
    case GL_DEBUG_TYPE_PUSH_GROUP:
        type_str = "Push Group";
        break;
    case GL_DEBUG_TYPE_POP_GROUP:
        type_str = "Pop Group";
        break;
    case GL_DEBUG_TYPE_OTHER:
        type_str = "Other";
        break;
    default:
        type_str = "Unknown";
    }

    const char *severity_str;
    switch (severity) {
    case GL_DEBUG_SEVERITY_HIGH:
        severity_str = "high";
        break;
    case GL_DEBUG_SEVERITY_MEDIUM:
        severity_str = "medium";
        break;
    case GL_DEBUG_SEVERITY_LOW:
        severity_str = "low";
        break;
    case GL_DEBUG_SEVERITY_NOTIFICATION:
        severity_str = "notification";
        break;
    default:
        severity_str = "Unknown";
    }

    Log::error(
        "[OpenGL error] ({}): {}\n"
        "Source: {}; Type: {}; Severity: {}",
        id, message, source_str, type_str, severity_str
    );
}
} // namespace
//
void enable_opengl_debug() {
    int flags;
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    bool has_debug = flags & GL_CONTEXT_FLAGS;
    Log::info("OpenGL has debug context: {}", has_debug);

    if (has_debug) {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(debug_message_callback, nullptr);
        glDebugMessageControl(
            GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE
        );
    }
}

Renderer::Renderer() {}

Renderer::~Renderer() {}

void Renderer::render(const Camera &camera) const {
    // TODO: make this an actual render function
    (void)camera;

    glClearColor(1, 0.4, 0.8, 1);
    glClear(GL_COLOR_BUFFER_BIT);
}

Renderer::Stats Renderer::stats() const { return stats_; }
