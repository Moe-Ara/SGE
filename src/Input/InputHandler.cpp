#include "InputHandler.h"
#include <algorithm>

namespace SGE::INPUT {

    std::vector<InputHandler *> InputHandler::m_instances;
    double InputHandler::m_mouseX = 0.0;
    double InputHandler::m_mouseY = 0.0;

    InputHandler::InputHandler(const std::vector<int>& keysToMonitor, const std::vector<int>& buttonsToMonitor)
            : m_enabled(true) {
        for (int key : keysToMonitor) {
            m_keys[key] = false;
        }
        for (int button : buttonsToMonitor) {
            m_buttons[button] = false;
        }
        m_instances.push_back(this);
    }

    InputHandler::~InputHandler() {
        m_instances.erase(std::remove(m_instances.begin(), m_instances.end(), this), m_instances.end());
    }

    bool InputHandler::isKeyPressed(unsigned int keycode) const {
        if (m_enabled) {
            auto it = m_keys.find(keycode);
            if (it != m_keys.end()) {
                return it->second;
            }
        }
        return false;
    }

    bool InputHandler::isMouseButtonPressed(unsigned int button) const {
        if (m_enabled) {
            auto it = m_buttons.find(button);
            if (it != m_buttons.end()) {
                return it->second;
            }
        }
        return false;
    }

    void InputHandler::setIsKeyPressed(int keycode, bool isPressed) {
        auto it = m_keys.find(keycode);
        if (it != m_keys.end()) {
            m_keys[keycode] = isPressed;
        }
    }

    void InputHandler::setIsButtonPressed(int button, bool isPressed) {
        auto it = m_buttons.find(button);
        if (it != m_buttons.end()) {
            m_buttons[button] = isPressed;
        }
    }

    void InputHandler::key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
        for (InputHandler *handler : m_instances) {
            handler->setIsKeyPressed(key, action != GLFW_RELEASE);
        }
    }

    void InputHandler::cursor_position_callback(GLFWwindow *window, double xpos, double ypos) {
        m_mouseX = xpos;
        m_mouseY = ypos;
    }

    void InputHandler::mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
        for (InputHandler *handler : m_instances) {
            handler->setIsButtonPressed(button, action != GLFW_RELEASE);
        }
    }

    void InputHandler::setupKeyHandler(SGE::GRAPHICS::Window& window) {
        GLFWwindow *glfwWindow = window.getMWindow();
        glfwSetKeyCallback(glfwWindow, InputHandler::key_callback);
        glfwSetCursorPosCallback(glfwWindow, InputHandler::cursor_position_callback);
        glfwSetMouseButtonCallback(glfwWindow, InputHandler::mouse_button_callback);
    }

    double InputHandler::getMouseX() {
        return m_mouseX;
    }

    double InputHandler::getMouseY() {
        return m_mouseY;
    }

} // namespace SGE::UTILS
