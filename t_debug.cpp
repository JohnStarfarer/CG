// Combined OpenGL program with debug callback
// Build: link with glfw3, glew, GL, e.g. (Linux):
// g++ main.cpp -lglfw -lGLEW -lGL -o app

#include <iostream>
#include <cstdio>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

using namespace std;

GLFWwindow *g_window = nullptr;

void init()
{
    // Set initial color of color buffer to blue.
    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
}

void reshape(GLFWwindow *window, int width, int height)
{
    // Adjust viewport on resize
    glViewport(0, 0, width, height);
}

void draw()
{
    // Принудительно вызвать ошибку: в core-профиле glDrawArrays без VAO -> GL_INVALID_OPERATION
    // glDrawArrays(GL_TRIANGLES, 0, 3);
    
    // Clear color buffer.
    glClear(GL_COLOR_BUFFER_BIT);
}

void cleanup()
{
}

// OpenGL debug callback
void GLAPIENTRY
MessageCallback(GLenum source,
                GLenum type,
                GLuint id,
                GLenum severity,
                GLsizei length,
                const GLchar* message,
                const void* userParam )
{
    (void)length;
    (void)userParam;

    const char *s_source, *s_type, *s_id, *s_severity;

    switch (source) {
        case GL_DEBUG_SOURCE_API:             s_source = "API"; break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   s_source = "WINDOW_SYSTEM"; break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER: s_source = "SHADER_COMPILER"; break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:     s_source = "THIRD_PARTY"; break;
        case GL_DEBUG_SOURCE_APPLICATION:     s_source = "APPLICATION"; break;
        case GL_DEBUG_SOURCE_OTHER:           s_source = "OTHER"; break;
        default:                              s_source = "UNKNOWN"; break;
    }

    switch (type) {
        case GL_DEBUG_TYPE_ERROR:               s_type = "ERROR"; break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: s_type = "DEPRECATED_BEHAVIOR"; break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  s_type = "UNDEFINED_BEHAVIOR"; break;
        case GL_DEBUG_TYPE_PORTABILITY:         s_type = "PORTABILITY"; break;
        case GL_DEBUG_TYPE_PERFORMANCE:         s_type = "PERFORMANCE"; break;
        case GL_DEBUG_TYPE_OTHER:               s_type = "OTHER"; break;
        case GL_DEBUG_TYPE_MARKER:              s_type = "MARKER"; break;
        default:                                s_type = "UNDEFINED"; break;
    }

    // id is application-specific; map common OpenGL object enums if desired
    s_id = "ID";

    switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH:         s_severity = "HIGH"; break;
        case GL_DEBUG_SEVERITY_MEDIUM:       s_severity = "MEDIUM"; break;
        case GL_DEBUG_SEVERITY_LOW:          s_severity = "LOW"; break;
        case GL_DEBUG_SEVERITY_NOTIFICATION: s_severity = "NOTIFICATION"; break;
        default:                             s_severity = "UNKNOWN"; break;
    }

    fprintf(stderr, "GL CALLBACK: %s source=%s (0x%x), type=%s (0x%x), severity=%s (0x%x), id=0x%x, message=%s\n",
            (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
            s_source, source,
            s_type, type,
            s_severity, severity,
            id,
            message);
}

bool initOpenGL()
{
    // Initialize GLFW
    if (!glfwInit())
    {
        cout << "Failed to initialize GLFW" << endl;
        return false;
    }

    // Request OpenGL 3.3 core profile and debug context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE); // request debug context

    // Create window.
    g_window = glfwCreateWindow(800, 600, "OpenGL Test (with debug)", NULL, NULL);
    if (g_window == NULL)
    {
        cout << "Failed to open GLFW window" << endl;
        glfwTerminate();
        return false;
    }

    // Make context current
    glfwMakeContextCurrent(g_window);

    // Set internal GLEW variable to activate OpenGL core profile.
    glewExperimental = true;

    // Initialize GLEW
    if (glewInit() != GLEW_OK)
    {
        cout << "Failed to initialize GLEW" << endl;
        return false;
    }

    // Ensure we can capture the escape key being pressed.
    glfwSetInputMode(g_window, GLFW_STICKY_KEYS, GL_TRUE);

    // Set callback for framebuffer resizing event.
    glfwSetFramebufferSizeCallback(g_window, reshape);

    // Enable debug output if supported
    if (glDebugMessageCallback) {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); // synchronous callback for easier debugging
        glDebugMessageCallback(MessageCallback, nullptr);
        // Enable all messages
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE,
                              0, nullptr, GL_TRUE);
    } else {
        cout << "GL debug callback not available." << endl;
    }

    return true;
}

void tearDownOpenGL()
{
    // Terminate GLFW.
    glfwTerminate();
}

int main()
{
    // Initialize OpenGL
    if (!initOpenGL())
        return -1;

    // Initialize graphical resources.
    init();

    // Main loop until window closed or escape pressed.
    while (glfwGetKey(g_window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
           !glfwWindowShouldClose(g_window))
    {
        // Draw scene.
        draw();

        // Swap buffers.
        glfwSwapBuffers(g_window);

        // Poll window events.
        glfwPollEvents();
    }

    // Cleanup graphical resources.
    cleanup();

    // Tear down OpenGL.
    tearDownOpenGL();

    return 0;
}
