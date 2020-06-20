#include "GLCanvas.h"
GLCanvas::GLCanvas(const int& w, const int& h){
    width = w;
    height = h;

    if (!glfwInit()) {
        throw std::runtime_error("Failed to initialize GLFW!");
    }

    glfwSetTime(0);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwWindowHint(GLFW_SAMPLES, 0);
    glfwWindowHint(GLFW_RED_BITS, 8);
    glfwWindowHint(GLFW_GREEN_BITS, 8);
    glfwWindowHint(GLFW_BLUE_BITS, 8);
    glfwWindowHint(GLFW_ALPHA_BITS, 8);
    glfwWindowHint(GLFW_STENCIL_BITS, 8);
    glfwWindowHint(GLFW_DEPTH_BITS, 24);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

    // Create a GLFWwindow object
    window = glfwCreateWindow(width, height, "OpenGL 3D", nullptr, nullptr);
    if (window == nullptr) {
        std::cerr << "Failed to create GLFW window!" << std::endl;
        glfwTerminate();
    }
    glfwMakeContextCurrent(window); 

    gl3wInit();

    glClearColor(0.f, 0.f, 0.f, 0.f);
    glClear(GL_COLOR_BUFFER_BIT);

    glEnable(GL_CULL_FACE);
    //glEnable(GL_DEPTH_TEST);
}

void GLCanvas::Clear() {
    while (GLenum error = glGetError()) {
        std::cerr << "OpenGL Error: " << error << std::endl;
    }
    glViewport(0, 0, width, height);
    glClearColor(0.f, 0.f, 0.f, 0.f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void GLCanvas::Paint() {
    glfwSwapInterval(0);
    glfwSwapBuffers(window);
    glfwPollEvents();
    glfwGetFramebufferSize(window, &width, &height);
}

GLCanvas::~GLCanvas() {
    glfwTerminate();
    glfwDestroyWindow(window);
}

int GLCanvas::GetWidth() {
    return width;
}

int GLCanvas::GetHeight() {
    return height;
}

int GLCanvas::ShouldClose() {
    return glfwWindowShouldClose(window);
}