#include "input_handler.h"
#include <algorithm>

namespace SGE::Input {

    std::vector<input_handler *> input_handler::m_instances;
    double input_handler::m_mouseX = 0.0;
    double input_handler::m_mouseY = 0.0;

    input_handler::input_handler(const std::vector<int>& keysToMonitor, const std::vector<int>& buttonsToMonitor)
            : m_enabled(true) {
        for (int key : keysToMonitor) {
            m_keys[key] = false;
        }
        for (int button : buttonsToMonitor) {
            m_buttons[button] = false;
        }
        m_instances.push_back(this);
    }

    input_handler::~input_handler() {
        m_instances.erase(std::remove(m_instances.begin(), m_instances.end(), this), m_instances.end());
    }

    bool input_handler::isKeyPressed(unsigned int keycode) const {
        if (m_enabled) {
            auto it = m_keys.find(keycode);
            if (it != m_keys.end()) {
                return it->second;
            }
        }
        return false;
    }

    bool input_handler::isMouseButtonPressed(unsigned int button) const {
        if (m_enabled) {
            auto it = m_buttons.find(button);
            if (it != m_buttons.end()) {
                return it->second;
            }
        }
        return false;
    }

    void input_handler::setIsKeyPressed(int keycode, bool isPressed) {
        auto it = m_keys.find(keycode);
        if (it != m_keys.end()) {
            m_keys[keycode] = isPressed;
        }
    }

    void input_handler::setIsButtonPressed(int button, bool isPressed) {
        auto it = m_buttons.find(button);
        if (it != m_buttons.end()) {
            m_buttons[button] = isPressed;
        }
    }

    void input_handler::key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
        for (input_handler *handler : m_instances) {
            handler->setIsKeyPressed(key, action != GLFW_RELEASE);
        }
    }

    void input_handler::cursor_position_callback(GLFWwindow *window, double xpos, double ypos) {
        m_mouseX = xpos;
        m_mouseY = ypos;
    }

    void input_handler::mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
        for (input_handler *handler : m_instances) {
            handler->setIsButtonPressed(button, action != GLFW_RELEASE);
        }
    }

    void input_handler::setupKeyHandler(SGE::graphics::Window *window) {
        GLFWwindow *glfwWindow = window->getMWindow();
        glfwSetKeyCallback(glfwWindow, input_handler::key_callback);
        glfwSetCursorPosCallback(glfwWindow, input_handler::cursor_position_callback);
        glfwSetMouseButtonCallback(glfwWindow, input_handler::mouse_button_callback);
    }

    double input_handler::getMouseX() {
        return m_mouseX;
    }

    double input_handler::getMouseY() {
        return m_mouseY;
    }

} // namespace SGE::utils
