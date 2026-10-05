#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

GLFWwindow* g_window;

void reshape(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);   
}

bool initOpenGL() {
    if (!glfwInit()) {
        std::cout << "GLFW Error\n";
        return false;
    }

    // оптимизация конвейра и исправление совместимостей (вроде)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    g_window = glfwCreateWindow(800, 600, "OpenGL", nullptr, nullptr);

    if (!g_window) {
        std::cout << "Window creation failed\n";
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(g_window);

    glewExperimental = true;

    if (glewInit() != GLEW_OK) {
        std::cout << "GLEW init failed\n";
        glfwTerminate();
        return false;
    }

    glfwSetInputMode(g_window, GLFW_STICKY_KEYS, GL_TRUE);
    glfwSetFramebufferSizeCallback(g_window, reshape);

    return true;

}

void tearDownOpenGL() {
    glfwTerminate();
}

void init() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}

void draw () {
    glClear(GL_COLOR_BUFFER_BIT);
}

void cleanup() {

}

int main() {
    if (!initOpenGL()) {
        return 1;
    }

    init();

    while (glfwWindowShouldClose(g_window) == 0) {
        // painting

        glfwSwapBuffers(g_window);
        glfwPollEvents();
    }

    cleanup();

    tearDownOpenGL();
    
    return 0;
}