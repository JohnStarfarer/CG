#include <cstdio>

#include <GL/glew.h>

// В основной программе необходимо добавить строки:
// glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
// ...
// glEnable(GL_DEBUG_OUTPUT);
// glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); // Необязательно, но позволяет точнее
// // поймать место возникновения ошибки
// glDebugMessageCallback(MessageCallback, nullptr);
// glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE,

// 0, nullptr, GL_TRUE);

void GLAPIENTRY
MessageCallback(GLenum source, 
                GLenum type,
                GLuint id,
                GLenum severity,
                GLsizei length,
                const GLchar* message,
                const void* userParam )
{
    (void)source;
    (void)id;
    (void)length;
    (void)userParam;
    
    char *s_source, *s_type, *s_id, *s_severity;

    switch (source) {
        case GL_DEBUG_SOURCE_API:
            s_source = "API";
            break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
            s_source = "WINDOW_SYSTEM";
            break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER:
            s_source = "SHADER_COMPILER";
            break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:
            s_source = "THIRD_PARTY";
            break;
        case GL_DEBUG_SOURCE_APPLICATION:
            s_source = "APPLICATION";
            break;
        case GL_DEBUG_SOURCE_OTHER:
            s_source = "OTHER";
            break;
        default:
            s_source = "UNKNOWN";
            break;
    }
    
    switch (type) {
        case GL_DEBUG_TYPE_ERROR:
            s_type = "ERROR";
            break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
            s_type = "DEPRECATED_BEHAVIOR";
            break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
            s_type = "UNDEFINED_BEHAVIOR";
            break;
        case GL_DEBUG_TYPE_PORTABILITY:
            s_type = "PORTABILITY";
            break;
        case GL_DEBUG_TYPE_PERFORMANCE:
            s_type = "PERFORMANCE";
            break;
        case GL_DEBUG_TYPE_OTHER:
            s_type = "OTHER";
            break;
        case GL_DEBUG_TYPE_MARKER:
            s_type = "MARKER";
            break;
        default:
            s_type = "UNDEFINED";
            break;
    }
    
    switch (id) {
        case GL_BUFFER:
            s_id = "BUFFER";
            break;
        case GL_SHADER:
            s_id = "SHADER";
            break;
        case GL_PROGRAM:
            s_id = "PROGRAM";
            break;
        case GL_QUERY:
            s_id = "QUERY";
            break;
        case GL_PROGRAM_PIPELINE:
            s_id = "PROGRAM_PIPELINE";
            break;
        case GL_SAMPLER:
            s_id = "SAMPLER";
            break;
        default:
            s_id = "UNKNOWN";
            break;
    }
    
    switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH:
            s_severity = "HIGH";
            break;
        case GL_DEBUG_SEVERITY_MEDIUM:
            s_severity = "MEDIUM";
            break;
        case GL_DEBUG_SEVERITY_LOW:
            s_severity = "LOW";
            break;
        case GL_DEBUG_SEVERITY_NOTIFICATION:
            s_severity = "NOTIFICATION";
            break;
        default:
            s_severity = "UNKNOWN";
            break;
    }
    
    fprintf(stderr, "GL CALLBACK: %s source = %s (0x%x), type = %s (0x%x),\
            severity = %s (0x%x), id = %s (0x%x), \message = %s\n",
            ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ),
            s_source, source,
            s_type, type,
            s_severity, severity,
            s_id, id,
            message );
}