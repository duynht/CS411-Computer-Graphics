#define _USE_MATH_DEFINES

#include <glad/glad.h>
//#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <chrono>
#include <cstdio>
#include <cmath>
#include <nanogui/nanogui.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "Shape.h"
#include "Shader.h"

nanogui::Screen* screen = nullptr;
int curMode, curColor, curTrans;
int cacheMode, cacheColor, cacheTrans;
double cursorX, cursorY;
int upArrowCount, downArrowCount, leftArrowCount, rightArrowCount;
double theta = 0;
double scaleFactor = 1;
std::vector<GLint> anchorX, anchorY;
glm::vec4 color = glm::vec4(1.0, 0.0, 0.0, 1.0);
glm::mat4 transformation = glm::mat4(); //identity matrix by default

static void resetGlobal() {
    theta = 0;
    scaleFactor = 1;
    anchorX.clear();
    anchorY.clear();
}

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
    new nanogui::Label(nanoguiWindow, "Interaction Mode", "sans-bold");
    nanoguiWindow->set_position(nanogui::Vector2i(0, 0));
    nanoguiWindow->set_layout(new nanogui::GroupLayout()); 
    nanogui::ComboBox* modeComboBox = new nanogui::ComboBox(nanoguiWindow, {
                                            " Idle ",
                                            " Draw ",
                                            " Transform "});

    modeComboBox->set_callback(
        [](int mode) {
            nanogui::MessageDialog* dialog = nullptr;
            switch (mode) {
            case 0:
                resetGlobal();
            case 1:
                dialog = new nanogui::MessageDialog(screen, nanogui::MessageDialog::Type::Information, "Draw Manual", "Use RIGHT-CLICK to add vertices to the polygon.\n Press ESC or switch to Idle Mode to delete the current polygon.");
                dialog->set_callback([](int result) { std::cout << "Dialog result: " << result << std::endl; });
                break;
            }
        }
    );

    new nanogui::Label(nanoguiWindow, "Color", "sans-bold");
    nanogui::ComboBox* colorComboBox = new nanogui::ComboBox(nanoguiWindow, {
                                        " Red ",
                                        " Green ",
                                        " Blue "});

    colorComboBox->set_callback(
        [](int selectedColor) {
            switch (selectedColor) {
            case 0:
                color = glm::vec4(1.0, 0.0, 0.0, 1.0);
                break;
            case 1:
                color = glm::vec4(0.0, 1.0, 0.0, 1.0);
                break;
            case 2:
                color = glm::vec4(0.0, 0.0, 1.0, 1.0);
            }
        }
    );


    new nanogui::Label(nanoguiWindow, "Transforms", "sans-bold");
    nanogui::ComboBox* transformComboBox = new nanogui::ComboBox(nanoguiWindow, {
                                         " Translate ",
                                         " Rotate ",
                                         " Scale " });

    transformComboBox->set_callback(
        [&nanoguiWindow](int selectedTransformation) {
            nanogui::MessageDialog* dialog = nullptr;
            nanogui::Button* leftButton = nullptr;
            nanogui::Button* rightButton = nullptr;
            nanogui::Popup* popup = nullptr;
            switch (selectedTransformation) {
            case 0:
                dialog = new nanogui::MessageDialog(screen, nanogui::MessageDialog::Type::Information, "Translation", "Use ARROW KEYS to translate the polygon around.");
                dialog->set_callback([](int result) { std::cout << "Dialog result: " << result << std::endl; });
                break;
            case 1:                
                dialog = new nanogui::MessageDialog(screen, nanogui::MessageDialog::Type::Information, "Rotation", "Click on pop-up buttons to rotate the polygon clockwise or count.");
                dialog->set_callback([](int result) { std::cout << "Dialog result: " << result << std::endl; });
                //popup = transformComboBox->popup();
                //popup->set_layout(new nanogui::GroupLayout());

                leftButton = nanoguiWindow->add<nanogui::Button>("Counterclockwise", FA_ARROW_CIRCLE_LEFT);
                leftButton->set_callback(
                    [] {
                        scaleFactor -= 0.1;
                    }
                );

                rightButton = nanoguiWindow->add<nanogui::Button>("Clockwise", FA_ARROW_CIRCLE_RIGHT);
                rightButton->set_callback(
                    [] {
                        scaleFactor += 0.1;
                    }
                );

            case 2:
                dialog = new nanogui::MessageDialog(screen, nanogui::MessageDialog::Type::Information, "Scale", "Use ARROW KEYS to translate the polygon around.");
                dialog->set_callback([](int result) { std::cout << "Dialog result: " << result << std::endl; });
            }
        }
    );
    
    screen->set_visible(true);
    screen->perform_layout();

    glfwSetCursorPosCallback(window,
        [](GLFWwindow*, double x, double y) {
            screen->cursor_pos_callback_event(x, y);
        }
    );

    glfwSetKeyCallback(window,
        [](GLFWwindow*, int key, int scancode, int action, int mods) {
            if (action == GLFW_PRESS && key == GLFW_KEY_ESCAPE) {
                resetGlobal();
            }
            if (action == GLFW_PRESS && curMode == 2) {
                if (curTrans == 0) {
                    switch (key) {
                    case GLFW_KEY_UP:
                        upArrowCount++;
                        break;
                    case GLFW_KEY_DOWN:
                        downArrowCount++;
                        break;
                    case GLFW_KEY_LEFT:
                        leftArrowCount++;
                        break;
                    case GLFW_KEY_RIGHT:
                        rightArrowCount++;
                        break;
                    }
                }
            }
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
                if (curMode == 1) {
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
    cacheMode = -1;
    curMode = -1;
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


        shader.setUniformVec4f("u_Color", color);

        curMode = modeComboBox->selected_index();
        curColor = colorComboBox->selected_index();
        curTrans = transformComboBox->selected_index();
        glfwGetCursorPos(window, &cursorX, &cursorY);

        transformation = glm::mat4(scaleFactor * glm::cos(theta), scaleFactor * -glm::sin(theta), 0.0, scaleFactor * (rightArrowCount - leftArrowCount),
                                   scaleFactor * glm::sin(theta),  scaleFactor * glm::cos(theta), 0.0,    scaleFactor * (downArrowCount - upArrowCount),
                                               0.0,              0.0,               scaleFactor * 1.0,                                              0.0,
                                               0.0,              0.0,                             0.0,                                scaleFactor * 1.0);
        
        shader.setUniformMat4f("u_transformation", transformation);


        if (curMode) {
            shape = new CGPolygon(anchorX, anchorY);
            shape->draw();
            delete shape;
            std::cout << "Polygon" << std::endl;
        }
        
        transformComboBox->set_visible(curMode == 2);
        
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
    delete colorComboBox;

    return 0;
}

