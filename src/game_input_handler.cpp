//
// Created by Mohamad on 20/04/2024.
//

#include <iostream>
#include "game_input_handler.h"


std::vector<glcpp::game_input_handler *> glcpp::game_input_handler::m_instances;
double glcpp::game_input_handler::m_mouseX, glcpp::game_input_handler::m_mouseY;

glcpp::game_input_handler::~game_input_handler() {
    m_instances.erase(std::remove(m_instances.begin(), m_instances.end(), this), m_instances.end());
}


bool glcpp::game_input_handler::isKeyPressed(unsigned int keycode) {
    bool result = false;
    if (m_enabled) {
        auto it = m_keys.find(keycode);
        if (it != m_keys.end()) {
            result = m_keys[keycode];
        }
    }
    return result;
}

bool glcpp::game_input_handler::isMouseButtonPressed(unsigned int button) {
    bool result = false;
    if (m_enabled) {
        auto it = m_buttons.find(button);
        if (it != m_buttons.end()) {
            result = m_buttons[button];
        }
    }
    return result;
}


//bool *glcpp::game_input_handler::getMKeys() {
//    return m_keys;
//}
//
//bool *glcpp::game_input_handler::getMMouseButtons() {
//    return m_mouseButtons;
//}
//
//double glcpp::game_input_handler::getMMouseX() {
//    return m_mouseX;
//}
//
//double glcpp::game_input_handler::getMMouseY() {
//    return m_mouseY;
//}
void glcpp::game_input_handler::key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    for (game_input_handler *keyInput: m_instances) {
        keyInput->setIsKeyPressed(key, action != GLFW_RELEASE);
    }

}

void glcpp::game_input_handler::cursor_position_callback(GLFWwindow *window, double xpos, double ypos) {
    glfwGetCursorPos(window, &m_mouseX, &m_mouseY);
}

void glcpp::game_input_handler::mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
    for (game_input_handler *keyInput: m_instances) {
        keyInput->setIsButtonPressed(button, action != GLFW_RELEASE);
    }
}

glcpp::game_input_handler::game_input_handler(std::vector<int> keysToMonitor, std::vector<int> buttonsToMonitor)
        : m_enabled(true) {
    for (int key: keysToMonitor) {
        m_keys[key] = false;
    }
    for (int button: buttonsToMonitor) {
        m_buttons[button] = false;
    }
    game_input_handler::m_instances.push_back(this);
}

void glcpp::game_input_handler::setIsKeyPressed(int keycode, bool isPressed) {
    auto it = m_keys.find(keycode);
    if (it != m_keys.end()) {
        m_keys[keycode] = isPressed;
    }
}

void glcpp::game_input_handler::setIsButtonPressed(int button, bool isPressed) {
    auto it = m_buttons.find(button);
    if (it != m_buttons.end()) {
        m_buttons[button] = isPressed;
    }
}


void glcpp::game_input_handler::setupKeyHandler(glcpp::game_window *window) {
    glfwSetKeyCallback(window->getMWindow(), game_input_handler::key_callback);
    glfwSetCursorPosCallback(window->getMWindow(), cursor_position_callback);
    glfwSetMouseButtonCallback(window->getMWindow(), mouse_button_callback);
}




