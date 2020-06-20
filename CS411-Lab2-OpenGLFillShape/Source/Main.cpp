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

int width, height;
nanogui::Screen* screen = nullptr;
int curMode, curShape;
double cursorX, cursorY;
Shape* shape;
glm::vec4 color;
std::vector<GLint> anchorX, anchorY;
std::vector<double> seed;
GLfloat* pixel;
GLuint pixelBuffer;
int index;
int nextIndex;

static void resetGlobal() {
    if (pixel) {
        delete[] pixel;
        pixel = nullptr;
    }
    anchorX.clear();
    anchorY.clear();
    seed.clear();
    index = nextIndex = 0;
}

int main() {
    width = height = 800;
    color = glm::vec4(1.0, 0.0, 0.0, 1.0);
    index = nextIndex = 0;
    pixel = nullptr;
    shape = nullptr;

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
    GLFWwindow* window = glfwCreateWindow(width, height, "Shape Fill", nullptr, nullptr);
    if (window == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        throw std::runtime_error("Could not initialize GLAD!");

    glCreateBuffers(1, &pixelBuffer);

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

    colorComboBox->set_callback(
        [](int mode) {
            switch (mode) {
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

    shapeComboBox->set_callback(
        [](int mode) {
            if (mode == 0) {
                if (curShape != 0) {
                    resetGlobal();
                    auto dialog = new nanogui::MessageDialog(screen, 
                                                            nanogui::MessageDialog::Type::Information,
                                                            "Circle", 
                                                            "Use RIGHT-CLICK to continuously choose your radii. \
                                                             The last point chosen will be the starting-point of a new radius.");
                    dialog->set_callback([](int result) { std::cout << "Dialog result: " << result << std::endl; });
                }
            }
            if (mode == 1) {
                if (curShape != 1) {
                    resetGlobal();
                    auto dialog = new nanogui::MessageDialog(screen,
                        nanogui::MessageDialog::Type::Information,
                        "Ellipse",
                        "Use RIGHT-CLICK to continuously choose your two foci and an arbitrary point on the ellipse. \
                         The fourth point chosen will erase the current ellipse and be the first new focus.");
                    dialog->set_callback([](int result) { std::cout << "Dialog result: " << result << std::endl; });
                }
            }
            if (mode == 2) {
                if (curShape != 2) {
                    resetGlobal();
                    auto dialog = new nanogui::MessageDialog(screen,
                        nanogui::MessageDialog::Type::Information,
                        "Rectangle",
                        "Use RIGHT-CLICK to continuously choose your main diagonals. \
                         The last point chosen will be the starting-point of a new diagonal.");
                    dialog->set_callback([](int result) { std::cout << "Dialog result: " << result << std::endl; });
                }
            }
            if (mode == 3) {
                if (curShape != 3) {
                    resetGlobal();
                    auto dialog = new nanogui::MessageDialog(screen,
                        nanogui::MessageDialog::Type::Information,
                        "Polygon",
                        "Use RIGHT-CLICK to add vertices to the polygon.");
                    dialog->set_callback([](int result) { std::cout << "Dialog result: " << result << std::endl; });
                }
            }

            curShape = mode;
        }
    );

    modeComboBox->set_callback(
        [&shapeComboBox](int mode) {
            if (mode == 0) {
                shapeComboBox->set_selected_index(0);
                shapeComboBox->set_enabled(true);

                auto dialog = new nanogui::MessageDialog(screen,
                    nanogui::MessageDialog::Type::Information,
                    "Circle",
                    "Use RIGHT-CLICK to continuously choose your radii. \
                                                             The last point chosen will be the starting-point of a new radius.");
                dialog->set_callback([](int result) { std::cout << "Dialog result: " << result << std::endl; });

                dialog = new nanogui::MessageDialog(screen, nanogui::MessageDialog::Type::Information, "Draw Mode", "Use RIGHT-CLICK to continuously add vertices to the shape to be drawn.");
                dialog->set_callback([](int result) { std::cout << "Dialog result: " << result << std::endl; });
                if (curMode != 0) {
                    resetGlobal();
                }
                curMode = 0;
            }

            if (mode == 1) {
                shapeComboBox->set_enabled(false);
                auto dialog = new nanogui::MessageDialog(screen, nanogui::MessageDialog::Type::Information, "Colorize Mode", "Use RIGHT-CLICK to pick the seed point for colorization.");
                dialog->set_callback([](int result) { std::cout << "Dialog result: " << result << std::endl; });
                curMode = 1;
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
                
                if (curMode == 1 && seed.empty()) {
                    seed.push_back(cursorX);
                    seed.push_back(cursorY);
                }
            }
            screen->mouse_button_callback_event(button, action, modifiers);
        }
    );


    auto dialog = new nanogui::MessageDialog(screen,
        nanogui::MessageDialog::Type::Information,
        "Circle",
        "Use RIGHT-CLICK to continuously choose your radii. \
                                                             The last point chosen will be the starting-point of a new radius.");
    dialog->set_callback([](int result) { std::cout << "Dialog result: " << result << std::endl; });

    dialog = new nanogui::MessageDialog(screen, nanogui::MessageDialog::Type::Information, "Draw Mode", "Use RIGHT-CLICK to continuously add vertices to the shape to be drawn.");
    dialog->set_callback([](int result) { std::cout << "Dialog result: " << result << std::endl; });

    // Game loop
    while (!glfwWindowShouldClose(window)) {
        //index = (index + 1) % 2;
        //nextIndex = (index + 1) % 2;

        std::cout << "OpenGL " << glGetString(GL_VERSION) << std::endl;

        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);

        shader.bind();
        glm::mat4 proj = glm::ortho(0.0, (double)width, (double)height, 0.0);
        shader.setUniformMat4f("u_MVP", proj);
        shader.setUniformVec4f("u_Color", color);      

        // Check if any events have been activated (key pressed, mouse moved etc.) and call corresponding response functions
        glfwPollEvents();
        glClearColor(0.f, 0.f, 0.f, 0.f);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwGetCursorPos(window, &cursorX, &cursorY);

        switch ((CGType)curShape) {
        case CGType::CIRCLE:
            if (anchorX.size() > 2) {
                anchorX.erase(anchorX.begin(), anchorX.end() - 2);
                anchorY.erase(anchorY.begin(), anchorY.end() - 2);
            }
            if (anchorX.size() == 2) {
                shape = new MidpointCircle(anchorX[0], anchorY[0], anchorX[1], anchorY[1]);
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
            }
            std::cout << "Rectangle" << std::endl;
            break;
        case CGType::POLYGON:
            if (!anchorX.empty() && !anchorY.empty()) {
                shape = new CGPolygon(anchorX, anchorY);
            }
            std::cout << "Polygon" << std::endl;
        }

        if (shape) {
            shape->draw();
            if (!seed.empty() && pixel) {
                shape->pixel = pixel;
                shape->fill(seed[0], seed[1], width, height);
            }

            delete shape;
            shape = nullptr;
        }

        
        if (curMode == 1 && !seed.empty() && !pixel) {
            //for (int i = 0; i < 2; i++) {
            //    glBindBuffer(GL_PIXEL_PACK_BUFFER, pixelBuffer[i]);
            //    glBufferData(GL_PIXEL_PACK_BUFFER, sizeof(GLfloat) * width * height * 4, 0, GL_STATIC_READ);

            //}
            //glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);

            glReadBuffer(GL_FRONT);
            glBindBuffer(GL_PIXEL_PACK_BUFFER, pixelBuffer);
            glBufferData(GL_PIXEL_PACK_BUFFER, sizeof(GLfloat) * width * height * 4, 0, GL_STATIC_READ);
            glReadPixels(0, 0, width, height, GL_RGBA, GL_FLOAT, 0);

            //glBindBuffer(GL_PIXEL_PACK_BUFFER, pixelBuffer[index]);
            //glReadPixels(0, 0, width, height, GL_RGBA, GL_FLOAT, 0);
            //glBindBuffer(GL_PIXEL_PACK_BUFFER, pixelBuffer[nextIndex]);

            GLfloat* _pixel = (GLfloat*)glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY);
            pixel = new GLfloat[width * height * 4];
            memcpy(pixel, _pixel, sizeof(GLfloat)* width* height * 4);
            glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
            glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
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
        //delete[] pixel;

    }

    // Terminate GLFW, clearing any resources allocated by GLFW.
    shader.unbind();
    glfwTerminate();
    nanogui::shutdown();
    resetGlobal();

    return 0;
}

