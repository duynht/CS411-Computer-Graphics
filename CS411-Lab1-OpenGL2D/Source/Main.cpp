#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <chrono>
#include <cstdio>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "Shape.h"
#include "Shader.h"

static void benchmark(Shape* shape) { 
    std::cout << "///////EXECUTION TIME///////" << std::endl;

    switch (shape->type) {
    case CGType::CIRCLE:
        std::cout << "!!!CIRCLE!!!" << std::endl;
        break;
    case CGType::ELLIPSE:
        std::cout << "!!!ELLIPSE!!!" << std::endl;
        break;
    case CGType::PARABOLA:
        std::cout << "!!!PARABOLA!!!" << std::endl;
        break;
    case CGType::HYPERBOLA:
        std::cout << "!!!HYPERBOLA!!!" << std::endl;
        break;
    case CGType::BRES_LINE:
    case CGType::DDA_LINE:
        std::cout << "!!!LINE!!!" << std::endl;
        std::cout << "OpenGL: ";
        auto glStart = std::chrono::high_resolution_clock::now();
        shape->glDraw();
        auto glEnd = std::chrono::high_resolution_clock::now();
        auto glDuration = std::chrono::duration_cast<std::chrono::microseconds>(glEnd - glStart);
        std::cout << glDuration.count() / 1000.0 << " ms" << std::endl;

    }
    auto start = std::chrono::high_resolution_clock::now();
    shape->draw();
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "Implemented: " << duration.count() / 1000.0 << " ms" << std::endl;
}

int main(void) {
    FILE* stream;
    std::vector<Shape*> shapes;
    CGType type;
    int n, arg1, arg2, arg3, arg4;

    freopen_s(&stream, "input.txt", "r", stdin);
    std::cin >> n;
    while (n--) {
        std::cin >> (int&)type;
        switch (type) {
        case CGType::DDA_LINE:
            std::cin >> arg1 >> arg2 >> arg3 >> arg4;
            shapes.push_back(new DDALine(arg1, arg2, arg3, arg4));
            break;
        case CGType::BRES_LINE:
            std::cin >> arg1 >> arg2 >> arg3 >> arg4;
            shapes.push_back(new BresenhamLine(arg1, arg2, arg3, arg4));
            break;
        case CGType::CIRCLE:
            std::cin >> arg1 >> arg2 >> arg3;
            shapes.push_back(new MidpointCircle(arg1, arg2, arg3));
            break;
        case CGType::ELLIPSE:
            std::cin >> arg1 >> arg2 >> arg3 >> arg4;
            shapes.push_back(new MidpointEllipse(arg1, arg2, arg3, arg4));
            break;
        case CGType::PARABOLA:
            std::cin >> arg1 >> arg2 >> arg3;
            shapes.push_back(new MidpointParabola(arg1, arg2, arg3, 800));
            break;
        case CGType::HYPERBOLA:
            std::cin >> arg1 >> arg2 >> arg3 >> arg4;
            shapes.push_back(new MidpointHyperbola(arg1, arg2, arg3, arg4, 800));
            break;
        }
    }
    
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(800, 800, "CS411-Lab1-OpenGL2D", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    gl3wInit();
    
    // OpenGL 4.5.0 - Build 24.20.100.6299    
    std::cout << "OpenGL " << glGetString(GL_VERSION) << std::endl;

    Shader shader("Source/resources/shaders/BasicShaders.glsl");
    //shader.bind();

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window)) {   

        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glm::mat4 proj = glm::ortho(0.0, (double) width, (double) height, 0.0);
        //setUniformMat4f(shader, "u_MVP", proj);
        shader.bind();
        shader.setUniformMat4f("u_MVP", proj);
        glViewport(0, 0, width, height);

        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);


        for (auto shape : shapes) {
            benchmark(shape);
        }

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
        system("cls");
    }
    shader.unbind();
    glfwTerminate();
    for (auto ptr : shapes) {
        delete ptr;
    }
    return 0;
}