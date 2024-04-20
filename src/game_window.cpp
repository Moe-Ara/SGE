//
// Created by Mohamad on 20/04/2024.
//

#include <iostream>
#include "game_window.h"
#include "game_input_handler.h"

namespace glcpp {

    void windowResize(GLFWwindow *window, int width, int height) {
        glViewport(0, 0, width, height);
    }

    game_window::game_window(const char *name, int width, int height)
            : m_title(name), m_width(width), m_height(height) {
        init();
    }

    game_window::~game_window() {
        glfwTerminate();
    }

    void game_window::update() {
        glfwPollEvents();

        glfwSwapBuffers(m_window);

    }

    void game_window::init() {
        if (!glfwInit()) {
            std::cerr << "ERROR: glfwInit FAILED" << std::endl;
            return;
        }
        m_window = glfwCreateWindow(m_width, m_height, m_title, NULL, NULL);
        if (!m_window) {
            std::cerr << "ERROR: Failed To Create Window" << std::endl;
            return;
        }
        glfwMakeContextCurrent(m_window);
        glfwSetWindowUserPointer(m_window, this);
        glfwSetWindowSizeCallback(m_window, windowResize);

        if (glewInit() != GLEW_OK) {
            std::cerr << "ERROR: glewInit FAILED" << std::endl;
            return;
        }
    }

    bool game_window::closed() const {
        return glfwWindowShouldClose(m_window) == 1;
    }

    void game_window::clear() const {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    GLFWwindow *game_window::getMWindow() const {
        return m_window;
    }

}