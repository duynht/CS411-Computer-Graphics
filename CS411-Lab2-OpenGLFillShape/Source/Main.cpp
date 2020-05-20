#include <glad/glad.h>
//#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <chrono>
#include <cstdio>
#include <nanogui/nanogui.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "Shape.h"
#include "Shader.h"

typedef std::pair<std::string, std::string> ShaderSource;
#define vs first
#define fs second

nanogui::Screen* screen = nullptr;
int curMode, curShape, curColor;
int cacheMode, cacheShape, cacheColor;
double cursorX, cursorY;
std::vector<GLint> anchorX, anchorY;

int main(int /* argc */, char** /* argv */) {

    if (!glfwInit()) {
        return -1;
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
    GLFWwindow* window = glfwCreateWindow(800, 800, "Shape Fill", nullptr, nullptr);
    if (window == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    //gl3wInit();

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        throw std::runtime_error("Could not initialize GLAD!");
    glGetError(); // pull and ignore unhandled errors like GL_INVALID_ENUM

    glClearColor(0.2f, 0.25f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

     // Create a nanogui screen and pass the glfw pointer to initialize
    screen = new nanogui::Screen();
    screen->initialize(window, true);

    Shader shader("Source/resources/shaders/BasicShaders.glsl");

    nanogui::Window* nanoguiWindow = new nanogui::Window(screen, "Dashboard");
    nanoguiWindow->set_position(nanogui::Vector2i(0, 0));
    nanoguiWindow->set_layout(new nanogui::GroupLayout()); 
    nanogui::ComboBox* modeComboBox = new nanogui::ComboBox(nanoguiWindow, {
                                            " Draw ",
                                            " Colorize "});
    nanogui::ComboBox* shapeComboBox = new nanogui::ComboBox(nanoguiWindow, { 
                                            " Circle ", 
                                            " Ellipse ", 
                                            " Rectangle ",
                                            " Polygon "});
    nanogui::ComboBox* colorComboBox = new nanogui::ComboBox(nanoguiWindow, {
                                        " Red ",
                                        " Green ",
                                        " Blue "});
    
    screen->set_visible(true);
    screen->perform_layout();

    glfwSetCursorPosCallback(window,
        [](GLFWwindow*, double x, double y) {
            screen->cursor_pos_callback_event(x, y);
        }
    );

    glfwSetKeyCallback(window,
        [](GLFWwindow*, int key, int scancode, int action, int mods) {
            screen->key_callback_event(key, scancode, action, mods);
        }
    );

    glfwSetCharCallback(window,
        [](GLFWwindow*, unsigned int codepoint) {
            screen->char_callback_event(codepoint);
        }
    );

    glfwSetDropCallback(window,
        [](GLFWwindow*, int count, const char** filenames) {
            screen->drop_callback_event(count, filenames);
        }
    );

    glfwSetScrollCallback(window,
        [](GLFWwindow*, double x, double y) {
            screen->scroll_callback_event(x, y);
        }
    );

    glfwSetFramebufferSizeCallback(window,
        [](GLFWwindow*, int width, int height) {
            screen->resize_callback_event(width, height);
        }
    );

    glfwSetMouseButtonCallback(window,
        [](GLFWwindow*, int button, int action, int modifiers) {
            if (button == GLFW_MOUSE_BUTTON_RIGHT
                && action == GLFW_PRESS) {
                if (curMode == 0) {
                    anchorX.push_back(cursorX);
                    anchorY.push_back(cursorY);
                }
                else {
                }
            }
            screen->mouse_button_callback_event(button, action, modifiers);
        }
    );

    Shape* shape;
    glm::vec4 color;

    // Game loop
    while (!glfwWindowShouldClose(window)) {
        std::cout << "OpenGL " << glGetString(GL_VERSION) << std::endl;
        shader.bind();
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);
        glm::mat4 proj = glm::ortho(0.0, (double)width, (double)height, 0.0);
        shader.setUniformMat4f("u_MVP", proj);

        // Check if any events have been activated (key pressed, mouse moved etc.) and call corresponding response functions
        glfwPollEvents();
        glClearColor(0.2f, 0.25f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        curMode = modeComboBox->selected_index();
        curShape = shapeComboBox->selected_index();
        curColor = colorComboBox->selected_index();
        glfwGetCursorPos(window, &cursorX, &cursorY);


        switch (curColor) {
        case 0:
            color = glm::vec4(1.0, 0.0, 0.0, 1.0); 
            break;
        case 1:
            color = glm::vec4(0.0, 1.0, 0.0, 1.0); 
            break;
        case 2:
            color = glm::vec4(0.0, 0.0, 1.0, 1.0); 
        }
        shader.setUniformVec4f("u_Color", color);

        if (curShape != cacheShape) {
            anchorX.clear();
            anchorY.clear();
            cacheShape = curShape;
        }

        switch ((CGType)curShape) {
        case CGType::CIRCLE:
            if (anchorX.size() > 2) {
                anchorX.erase(anchorX.begin(), anchorX.end() - 2);
                anchorY.erase(anchorY.begin(), anchorY.end() - 2);
            }
            if (anchorX.size() == 2) {
                shape = new MidpointCircle(anchorX[0], anchorY[0], anchorX[1], anchorY[1]);
                shape->draw();
                delete shape;
            }
            std::cout << "Circle" << std::endl;
            break;
        case CGType::ELLIPSE:
            if (anchorX.size() > 3) {
                anchorX.erase(anchorX.begin(), anchorX.end() - 1);
                anchorY.erase(anchorY.begin(), anchorY.end() - 1);
            }
            if (anchorX.size() == 3) {
                shape = new MidpointEllipse(anchorX[0], anchorY[0], anchorX[1], anchorY[1], anchorX[2], anchorY[2]);
                shape->draw();
                delete shape;
            }
            std::cout << "Ellipse" << std::endl;
            break;
        case CGType::RECTANGLE:
            if (anchorX.size() > 2) {
                anchorX.erase(anchorX.begin(),anchorX.end()-2);
                anchorY.erase(anchorY.begin(),anchorY.end()-2);
            }
            if (anchorX.size() == 2) {
                shape = new CGRectangle(anchorX[0], anchorY[0], anchorX[1], anchorY[1]);
                shape->draw();
                delete shape;
            }
            std::cout << "Rectangle" << std::endl;
            break;
        case CGType::POLYGON:
            shape = new CGPolygon(anchorX, anchorY);
            shape->draw();
            delete shape;
            std::cout << "Polygon" << std::endl;
        }
        
        for (auto x : anchorX) {
            std::cout << x << " ";
        }
        std::cout << std::endl;
        for (auto y : anchorY) {
            std::cout << y << " ";
        }
        std::cout << std::endl;
        // Draw nanogui
        screen->draw_widgets();

        glfwSwapInterval(0);
        glfwSwapBuffers(window);

        system("cls");
    }

    // Terminate GLFW, clearing any resources allocated by GLFW.
    shader.unbind();
    glfwTerminate();

    delete modeComboBox;
    delete shapeComboBox;
    delete colorComboBox;

    return 0;
}

