//
// Created by Mohamad on 20/04/2024.
//

#ifndef GLCPP_GAME_INPUT_HANDLER_H
#define GLCPP_GAME_INPUT_HANDLER_H

#include <GL/glew.h>

#include <GLFW/glfw3.h>
#include <vector>
#include <map>
#include "game_window.h"

namespace glcpp {

    class game_input_handler {
    public:
        game_input_handler(std::vector<int> keysToMonitor, std::vector<int> buttonsToMonitor);

        ~game_input_handler();

        bool isKeyPressed(unsigned int keycode);

        bool isMouseButtonPressed(unsigned int button);

        bool getIsEnabled() { return m_enabled; }

        void setIsEnabled(bool value) { m_enabled = value; }

        static double m_mouseX, m_mouseY;
    private:
        bool m_enabled;

        void setIsKeyPressed(int keycode, bool isPressed);

        std::map<int, bool> m_keys;
        std::map<int, bool> m_buttons;

    public:
        static void setupKeyHandler(glcpp::game_window *window);

    private:
        static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);

        static void cursor_position_callback(GLFWwindow *window, double xpos, double ypos);

        static void mouse_button_callback(GLFWwindow *window, int button, int action, int mods);

        static std::vector<game_input_handler *> m_instances;

        void setIsButtonPressed(int i, bool b);
    };
}


#endif //GLCPP_GAME_INPUT_HANDLER_H
