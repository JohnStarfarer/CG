#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

GLFWwindow* g_window;

struct Model {
    GLuint vao; // vertex array object
    GLuint vbo; // vertex buffer
    GLuint ibo; // index buffer
    GLuint index_count;
    GLuint g_model_shade;

};
Model g_model = { 0 };


// GLuint createShader(const GLchar* code, GLenum type) {
//     GLuint result = glCreateShader(type);

//     glShaderSource(result, 1, &code, nullptr);
//     GLint status = 0;

//     glCompileShader(result);

//     glGetShaderiv(result, GL_COMPILE_STATUS, &status);

//     if (!status) {
//         GLint len = 0;
//         glGetShaderiv(result, GL_INFO_LOG_LENGTH, &len);

//         if (len > 0) {
//             GLchar* message = new GLchar[len+1];
//             glGetShaderInfoLog(result, len, nullptr, message);
//             std::cout << "Shader compile error: " << message << std::endl;
//             delete[] message;
//         }

//         glDeleteShader(result);

//         return 0;
//     }
// }

// void createShaderProgram() {
//     g_model_shader = 0;

//     const GLchar vsh[] = R"(
//     )";

//     const GLchar fsh[] = R"(
//     )";

//     GLuint vertexShader, fragmentShader;
//     vertexShader = createShader(vsh, GL_VERTEX_SHADER);
//     fragmentShader = createShader(fsh, GL_FRAGMENT_SHADER);

//     g_model.shader = createProgram(vertexShader, fragmentShader);

// }

void createMode() {
    const GLfloat vertices[] = {
        -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
         0.5f, -0.5f, 0.0f, 0.0f, 1.0f,
         0.0f,  0.5f, 0.0f, 1.0f, 0.0f,
    };

    // GLunit GLushort
    const GLuint indices[] = { 
        0, 1, 2,
    };

    g_model.index_count = 3;

    glGenVertexArrays(1, &g_model.vao );
    glBindVertexArray(g_model.vao);

    glGenBuffers(1, &g_model.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, g_model.vbo);
    glBufferData(GL_ARRAY_BUFFER, 15 * sizeof(GLfloat), vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &g_model.ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_model.ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 15 * sizeof(GLuint), vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));
}

void init() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}

void reshape(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);   
}

void draw () {
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(g_model.shader);
    glBindVertexArray(g_model.vao);

    glDrawElements(GL_TRIANGLES, g_model.index_count, GL_UNSIGNED_INT, nullptr);
}

void cleanup() {
    glDeleteBuffers(1, &g_model.ibo);
    glDeleteBuffers(1, &g_model.vbo);
    glDeleteVertexArrays(1, &g_model.vao);
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

void createShaderProgram() {
    g_model.shader = 0;

    // вершинный шейдер
    const GLchar vsh[] = R"(
        #version 300

        layout(location = 0) in vec2 a_position;
        layout(location = 1) in vec3 a_color;

        out vec3 v_color;

        void main() {
            v_collor = a_color;
            gl_Position = vec4(a_position, 0.0, 1.0)
        }
    )";

    // фрагметный шейдер
    const GLchar fsh[] = R"(
        #version 330

        in vec3 v_color;

        out vec3 o_color;

        layout(location = 0) out vec4 o_color;

        void main() {
            o_color = vec4(v_color, 1.0);
        }
    )";

    GLuint vertexShader, fragmentShader;
    vertexShader = createShader(vsh, GL_VERTEX_SHADER);
    fragmentShader = createShader(fsh, GL_FRAGMENT_SHADER);

    g_model.shader = createProgram(vertexShader, fragmentShader);

}

GLuint createShader(const GLchar* code, GLenum type) {
    GLuint result = glCreateShader(type);

    glShaderSource(result, 1, &code, nullptr);
    GLint status = 0;

    glCompileShader(result);

    glGetShaderiv(result, GL_COMPILE_STATUS, &status);

    if (!status) {
        GLint len = 0;
        glGetShaderiv(result, GL_INFO_LOG_LENGTH, &len);

        if (len > 0) {
            GLchar* message = new GLchar[len+1];
            glGetShaderInfoLog(result, len, nullptr, message);
            std::cout << "Shader compile error: " << message << std::endl;
            delete[] message;
        }

        glDeleteShader(result);

        return 0;
    }
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