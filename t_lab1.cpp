// main.cpp
// Build: g++ main.cpp -lglfw -lGLEW -lGL -o patterns

#include <iostream>
#include <string>
#include <cstdio>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
using namespace std;

const char* vs_src = R"GLSL(
#version 330 core
layout(location = 0) in vec2 aPos; // NDC [-1,1]
out vec2 uv; // [0,1]
void main() {
    uv = aPos * 0.5 + 0.5;
    gl_Position = vec4(aPos, 0.0, 1.0);
}
)GLSL";

// Жёсткий волнистый узор: sign(sin(...)) -> чёрно-белые полосы, синус меняет x по y -> реальные волны
const char* fs_wave = R"GLSL(
#version 330 core
in vec2 uv;
out vec4 FragColor;
uniform vec2 resolution;
void main() {
    // центрируем и корректируем по аспекту
    vec2 p = (uv - 0.5) * vec2(resolution.x / resolution.y, 1.0);
    // частота полос
    float freq = 10.0;
    // синус по y даёт горизонтальные волны; небольшая модуляция по x для искривления
    float value = sin(p.y * freq + sin(p.x * 6.0) * 1.2);
    // жёсткий порог: >0 -> белое, <=0 -> чёрное
    float bw = value > 0.0 ? 1.0 : 0.0;
    FragColor = vec4(vec3(bw), 1.0);
}
)GLSL";

// Жёсткие кольца: sign(cos(r * freq))
const char* fs_rings = R"GLSL(
#version 330 core
in vec2 uv;
out vec4 FragColor;
uniform vec2 resolution;
void main() {
    vec2 p = (uv - 0.5) * vec2(resolution.x / resolution.y, 1.0);
    float r = length(p);
    float freq = 20.0;
    float value = cos(r * freq);
    float bw = value > 0.0 ? 1.0 : 0.0;
    FragColor = vec4(vec3(bw), 1.0);
}
)GLSL";

static void glfw_err(int e, const char* d){ fprintf(stderr,"GLFW err %d: %s\n", e, d); }

GLuint compile_shader(GLenum type, const char* src) {
    GLuint s = glCreateShader(type);
    glShaderSource(s, 1, &src, nullptr);
    glCompileShader(s);
    GLint ok; glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
    if (!ok) { char log[1024]; glGetShaderInfoLog(s, 1024, nullptr, log); cerr << log << endl; glDeleteShader(s); return 0; }
    return s;
}

GLuint link_program(GLuint vs, GLuint fs) {
    GLuint p = glCreateProgram();
    glAttachShader(p, vs); glAttachShader(p, fs);
    glLinkProgram(p);
    GLint ok; glGetProgramiv(p, GL_LINK_STATUS, &ok);
    if (!ok) { char log[1024]; glGetProgramInfoLog(p, 1024, nullptr, log); cerr << log << endl; glDeleteProgram(p); return 0; }
    return p;
}

int main(int argc, char** argv) {
    string mode = "wave";
    if (argc > 1) mode = argv[1];
    if (mode != "wave" && mode != "rings") { cerr << "Usage: " << argv[0] << " [wave|rings]\n"; return 1; }

    glfwSetErrorCallback(glfw_err);
    if (!glfwInit()) return 1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* win = glfwCreateWindow(800, 800, "Wave / Rings", nullptr, nullptr);
    if (!win) { glfwTerminate(); return 1; }
    glfwMakeContextCurrent(win);
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) { cerr << "GLEW init failed\n"; return 1; }

    // Rectangle via TRIANGLE_STRIP (4 vertices)
    float verts[] = {
        -1.0f, -1.0f, // BL
        -1.0f,  1.0f, // TL
         1.0f, -1.0f, // BR
         1.0f,  1.0f  // TR
    };

    GLuint vao=0, vbo=0;
    glGenVertexArrays(1,&vao);
    glGenBuffers(1,&vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,2*sizeof(float),(void*)0);
    glBindVertexArray(0);

    GLuint vs = compile_shader(GL_VERTEX_SHADER, vs_src);
    const char* fs_src = (mode=="wave") ? fs_wave : fs_rings;
    GLuint fs = compile_shader(GL_FRAGMENT_SHADER, fs_src);
    if (!vs || !fs) return 1;
    GLuint prog = link_program(vs, fs);
    glDeleteShader(vs); glDeleteShader(fs);
    if (!prog) return 1;

    GLint locRes = glGetUniformLocation(prog, "resolution");

    while (!glfwWindowShouldClose(win)) {
        int w,h; glfwGetFramebufferSize(win,&w,&h);
        glViewport(0,0,w,h);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(prog);
        if (locRes >= 0) glUniform2f(locRes, (float)w, (float)h);

        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); // 4 вертекса, <=6
        glBindVertexArray(0);

        glfwSwapBuffers(win);
        glfwPollEvents();
        if (glfwGetKey(win, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(win,1);
    }

    glDeleteProgram(prog);
    glDeleteBuffers(1,&vbo);
    glDeleteVertexArrays(1,&vao);
    glfwDestroyWindow(win);
    glfwTerminate();
    return 0;
}
