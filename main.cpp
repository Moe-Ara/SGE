#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "src/game_object.h"
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "src/game_model.h"
#include "src/game_window.h"
#include "src/game_input_handler.h"

void errorCallback(int error, const char *description) {
    std::cerr << "Error: " << description << std::endl;
}

int main() {
    // Set GLFW error callback
    glfwSetErrorCallback(errorCallback);
    glcpp::game_window window("SGE", 960, 540);
    glcpp::game_input_handler::setupKeyHandler(&window);

    glcpp::game_input_handler inputHandler({GLFW_KEY_A,
                                            GLFW_KEY_S,
                                            GLFW_KEY_W,
                                            GLFW_KEY_D,
                                            GLFW_KEY_LEFT_CONTROL,
                                            GLFW_KEY_LEFT_SHIFT,
                                            GLFW_KEY_Q,
                                            GLFW_KEY_E},
                                           {GLFW_MOUSE_BUTTON_RIGHT, GLFW_MOUSE_BUTTON_LEFT});


    glClearColor(0.f, 0.2f, 0.3f, 1.0f);
    float x = 0.1;
    while (!window.closed()) {
        window.clear();
        if (inputHandler.isKeyPressed(GLFW_KEY_A)) {

            std::cout << "PRESSED" << std::endl;

        }
        if (inputHandler.isMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT)) {
            std::cout << "Button Works" << std::endl;
        }
//        glBegin(GL_QUADS);
//        glVertex2f(-0.5f,-0.5f);
//        glVertex2f(-0.5f,0.5f);
//        glVertex2f(0.5f,0.5f);
//        glVertex2f(0.5f,-0.5f);

        glEnd();
        window.update();
    }

    return 0;
}
