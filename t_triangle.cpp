// triangle.cpp
#include <iostream>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

using namespace std;

GLFWwindow *g_window = nullptr;

void reshape(GLFWwindow* /*win*/, int width, int height) {
    glViewport(0, 0, width, height);
}

GLuint compileShader(GLenum type, const char* src) {
    GLuint sh = glCreateShader(type);
    glShaderSource(sh, 1, &src, nullptr);
    glCompileShader(sh);
    GLint ok = 0;
    glGetShaderiv(sh, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        GLint len = 0;
        glGetShaderiv(sh, GL_INFO_LOG_LENGTH, &len);
        vector<char> log(len+1);
        glGetShaderInfoLog(sh, len, nullptr, log.data());
        cerr << "Shader compile error: " << log.data() << endl;
        glDeleteShader(sh);
        return 0;
    }
    return sh;
}

GLuint createProgram(const char* vs_src, const char* fs_src) {
    GLuint vs = compileShader(GL_VERTEX_SHADER, vs_src);
    GLuint fs = compileShader(GL_FRAGMENT_SHADER, fs_src);
    if (!vs || !fs) return 0;
    GLuint prog = glCreateProgram();
    glAttachShader(prog, vs);
    glAttachShader(prog, fs);
    glLinkProgram(prog);
    GLint ok = 0;
    glGetProgramiv(prog, GL_LINK_STATUS, &ok);
    if (!ok) {
        GLint len = 0;
        glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &len);
        vector<char> log(len+1);
        glGetProgramInfoLog(prog, len, nullptr, log.data());
        cerr << "Program link error: " << log.data() << endl;
        glDeleteProgram(prog);
        prog = 0;
    }
    glDeleteShader(vs);
    glDeleteShader(fs);
    return prog;
}

bool initOpenGL() {
    if (!glfwInit()) {
        cerr << "Failed to initialize GLFW\n";
        return false;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    g_window = glfwCreateWindow(800, 600, "Triangle", nullptr, nullptr);
    if (!g_window) {
        cerr << "Failed to create window\n";
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(g_window);
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        cerr << "Failed to init GLEW\n";
        return false;
    }
    glfwSetFramebufferSizeCallback(g_window, reshape);
    glfwSetInputMode(g_window, GLFW_STICKY_KEYS, GL_TRUE);
    return true;
}

int main() {
    if (!initOpenGL()) return -1;

    // White background
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

    // Simple vertex + fragment shaders (positions only)
    const char* vs = R"(
        #version 330 core
        layout(location = 0) in vec3 aPos;
        layout(location = 1) in vec3 aColor;
        out vec3 vColor;
        void main() {
            vColor = aColor;
            gl_Position = vec4(aPos, 1.0);
        }
    )";

    const char* fs = R"(
        #version 330 core
        in vec3 vColor;
        out vec4 FragColor;
        void main() {
            FragColor = vec4(vColor, 1.0);
        }
    )";

    GLuint program = createProgram(vs, fs);
    if (!program) return -1;

    // Triangle vertex positions and colors (NDC)
    float vertices[] = {
        // positions        // colors
         0.0f,  0.5f, 0.0f,  1.0f, 0.0f, 0.0f, // top (red)
        -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f, // left (green)
         0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f  // right (blue)
    };

    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
      glBindBuffer(GL_ARRAY_BUFFER, VBO);
      glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
      // position
      glEnableVertexAttribArray(0);
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
      // color
      glEnableVertexAttribArray(1);
      glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glBindVertexArray(0);

    // ensure viewport initially set
    int w, h;
    glfwGetFramebufferSize(g_window, &w, &h);
    reshape(nullptr, w, h);

    // Main loop
    while (glfwGetKey(g_window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
           !glfwWindowShouldClose(g_window))
    {
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(program);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(g_window);
        glfwPollEvents();
    }

    // cleanup
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
    glDeleteProgram(program);
    glfwDestroyWindow(g_window);
    glfwTerminate();
    return 0;
}
