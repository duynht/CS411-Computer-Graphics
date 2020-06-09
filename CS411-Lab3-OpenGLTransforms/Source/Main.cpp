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
#include <numeric>
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
    nanogui::init();
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
    GLFWwindow* window = glfwCreateWindow(800, 800, "Transformation", nullptr, nullptr);
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

    Shader shader("../Source/resources/shaders/BasicShaders.glsl");

    nanogui::Window* nanoguiWindow = new nanogui::Window(screen, "Dashboard");
    new nanogui::Label(nanoguiWindow, "Interaction Mode", "sans-bold");
    nanoguiWindow->set_position(nanogui::Vector2i(0, 0));
    nanoguiWindow->set_layout(new nanogui::GroupLayout()); 
    nanogui::ComboBox* modeComboBox = new nanogui::ComboBox(nanoguiWindow, {
                                            " Idle ",
                                            " Draw ",
                                            " Transform "});

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

    nanogui::Widget* rotateGroup = new nanogui::Widget(nanoguiWindow);
    rotateGroup->set_layout(new nanogui::BoxLayout(nanogui::Orientation::Horizontal, nanogui::Alignment::Fill, 0, 6));
    rotateGroup->set_layout(new nanogui::BoxLayout(nanogui::Orientation::Horizontal, nanogui::Alignment::Fill, 0, 6));
    auto ccwButton = new nanogui::Button(rotateGroup, " CCW ", FA_UNDO);
    auto cwButton = new nanogui::Button(rotateGroup, "  CW  ", FA_REDO);

    //ccwButton->set_width((transformComboBox->width() - 6) / 2);
    ccwButton->set_callback(
        [] {
            theta +=  M_PI / 180;
        }
    );
    ccwButton->set_enabled(false);

    //cwButton->set_width((transformComboBox->width() - 6) / 2);
    cwButton->set_callback(
        [] {
            theta -= M_PI / 180;
        }
    );
    cwButton->set_enabled(false);

    nanogui::Widget* scaleGroup = new nanogui::Widget(nanoguiWindow);
    scaleGroup->set_layout(new nanogui::BoxLayout(nanogui::Orientation::Horizontal, nanogui::Alignment::Middle, 0, 6));
    auto scaledownButton = new nanogui::Button(scaleGroup, "", FA_MINUS);
    auto currentScaleTextBox = new nanogui::TextBox(scaleGroup);
    auto scaleupButton = new nanogui::Button(scaleGroup, "", FA_PLUS);

    currentScaleTextBox->set_value("100");
    currentScaleTextBox->set_units("%");

    scaleupButton->set_callback(
        [&currentScaleTextBox] {
            scaleFactor += 0.1;
            //currentScaleTextBox->set_value(std::to_string((int)(scaleFactor * 100)));
        }
    );
    scaleupButton->set_enabled(false);

    scaledownButton->set_callback(
        [&currentScaleTextBox] {
            scaleFactor -= 0.1;
            //currentScaleTextBox->set_value(std::to_string((int)(scaleFactor * 100)));
        }
    );
    scaledownButton->set_enabled(false);

    transformComboBox->set_callback(
        [&nanoguiWindow, &ccwButton, &cwButton, &scaleupButton, &scaledownButton](int selectedTransformation) {
            ccwButton->set_enabled(false);
            cwButton->set_enabled(false);
            scaleupButton->set_enabled(false);
            scaledownButton->set_enabled(false);
            if (selectedTransformation == 0) {
                auto dialog = new nanogui::MessageDialog(screen, nanogui::MessageDialog::Type::Information, "Translation", "Use ARROW KEYS to translate the polygon around.");
                dialog->set_callback([](int result) { std::cout << "Dialog result: " << result << std::endl; });
            }
            if (selectedTransformation == 1) {
                ccwButton->set_enabled(true);
                cwButton->set_enabled(true);
                auto dialog = new nanogui::MessageDialog(screen, nanogui::MessageDialog::Type::Information, "Rotation", "Click CCW/CW BUTTONS to rotate the polygon clockwise or counterclockwise relative to the screen center.");
                dialog->set_callback([](int result) { std::cout << "Dialog result: " << result << std::endl; });\
            }
            if (selectedTransformation == 2) {
                scaleupButton->set_enabled(true);
                scaledownButton->set_enabled(true);
                auto dialog = new nanogui::MessageDialog(screen, nanogui::MessageDialog::Type::Information, "Scaling", "Click +/- BUTTONS to scale the polygon");
                dialog->set_callback([](int result) { std::cout << "Dialog result: " << result << std::endl; });
            }
        }
    );

    transformComboBox->set_enabled(false);

    modeComboBox->set_callback(
        [&transformComboBox, &currentScaleTextBox, &ccwButton, &cwButton, &scaleupButton, &scaledownButton](int mode) {
            transformComboBox->set_enabled(false);
            ccwButton->set_enabled(false);
            cwButton->set_enabled(false);
            scaleupButton->set_enabled(false);
            scaledownButton->set_enabled(false);
            if (mode == 0) {
                resetGlobal();
            }
            if (mode == 1) {
                if (curMode != 1) {
                    resetGlobal();
                    auto dialog = new nanogui::MessageDialog(screen, nanogui::MessageDialog::Type::Information, "Draw Mode", "Use RIGHT-CLICK to add vertices to the polygon.\n Press ESC or switch to Idle Mode to delete the current progress.");
                    dialog->set_callback([](int result) { std::cout << "Dialog result: " << result << std::endl; });
                }

            }
            if (mode == 2) {
                transformComboBox->set_enabled(true);
                transformComboBox->set_selected_index(0);
                auto dialog = new nanogui::MessageDialog(screen, nanogui::MessageDialog::Type::Information, "Translation", "Use ARROW KEYS to translate the polygon around.");
                dialog->set_callback([](int result) { std::cout << "Dialog result: " << result << std::endl; });
                
                dialog = new nanogui::MessageDialog(screen, nanogui::MessageDialog::Type::Information, "Transform Mode", "Choose your transformation in the drop-down menu.\n Press ESC or switch to any mode to delete the current progress.");
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

        double offsetX = std::accumulate(anchorX.begin(), anchorX.end(), 0.0) / anchorX.size();
        double offsetY = std::accumulate(anchorY.begin(), anchorY.end(), 0.0) / anchorY.size();

        std::cout << std::endl << offsetX << " " << offsetY << std::endl;

        //offsetX /= width;
        //offsetY /= height;

        transformation = glm::mat4( glm::cos(theta), -glm::sin(theta), 0.0, (-offsetX * glm::cos(theta) + offsetY * glm::sin(theta) + offsetX) * (((double) rightArrowCount - (double) leftArrowCount)/(double) width),
                                    glm::sin(theta),  glm::cos(theta), 0.0, (-offsetX * glm::sin(theta) - offsetY * glm::cos(theta) + offsetY) *  (((double) upArrowCount - (double) downArrowCount)/(double) height),
                                                0.0,              0.0,                                                              1.0,                                                                  0.0,
                                                0.0,              0.0,                                                              0.0,                                                      1.0/scaleFactor);
        
        shader.setUniformMat4f("u_transformation", transformation);

        currentScaleTextBox->set_value(std::to_string((int)(scaleFactor * 100)));

        if (curMode) {
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
    nanogui::shutdown();

    return 0;
}

