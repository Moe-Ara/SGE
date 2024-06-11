#ifndef GLCPP_SGE_INPUT_HANDLER_H
#define GLCPP_SGE_INPUT_HANDLER_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <map>
#include "sge_window.h"

namespace SGE::utils {

    class sge_input_handler {
    public:
        sge_input_handler(const std::vector<int>& keysToMonitor, const std::vector<int>& buttonsToMonitor);
        ~sge_input_handler();

        bool isKeyPressed(unsigned int keycode) const;
        bool isMouseButtonPressed(unsigned int button) const;

        bool getIsEnabled() const { return m_enabled; }
        void setIsEnabled(bool value) { m_enabled = value; }

        static void setupKeyHandler(SGE::graphics::sge_window *window);
        static double getMouseX();  // Getter function for m_mouseX
        static double getMouseY();   // Getter function for m_mouseY


    private:
        void setIsKeyPressed(int keycode, bool isPressed);
        void setIsButtonPressed(int button, bool isPressed);

        bool m_enabled;
        std::map<int, bool> m_keys;
        std::map<int, bool> m_buttons;

        static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
        static void cursor_position_callback(GLFWwindow *window, double xpos, double ypos);
        static void mouse_button_callback(GLFWwindow *window, int button, int action, int mods);

        static std::vector<sge_input_handler *> m_instances;
        static double m_mouseX, m_mouseY;
    };

} // namespace SGE::utils

#endif // GLCPP_SGE_INPUT_HANDLER_H
