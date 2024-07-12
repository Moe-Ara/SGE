#include "Window.h"
#include "../Exceptions/SGE_Exception.h"
#include <iostream>

namespace SGE::GRAPHICS {

    Window& Window::getInstance(const std::string& title, int width, int height) {
        static Window instance(title, width, height);
        return instance;
    }

    Window::Window(const std::string& title, int width, int height)
            : m_title(title), m_width(width), m_height(height), m_window(nullptr) {
        init();
    }

    Window::~Window() {
        if (m_window) {
            glfwDestroyWindow(m_window);
        }
        glfwTerminate();
    }

    void Window::init() {
        if (!glfwInit()) {
            throw SGE::EXCEPTIONS::SGE_Exception("Failed to initialize GLFW");
        }

        m_window = glfwCreateWindow(m_width, m_height, m_title.c_str(), nullptr, nullptr);
        if (!m_window) {
            glfwTerminate();
            throw SGE::EXCEPTIONS::SGE_Exception("Failed to create GLFW window");
        }

        glfwMakeContextCurrent(m_window);
        glfwSetWindowUserPointer(m_window, this);
        glfwSetWindowSizeCallback(m_window, windowResize);

        if (glewInit() != GLEW_OK) {
            throw SGE::EXCEPTIONS::SGE_Exception("Failed to initialize GLEW");
        }
    }

    void Window::update() {
        glfwPollEvents();
        glfwSwapBuffers(m_window);
    }

    void Window::clear() const {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    bool Window::closed() const {
        return glfwWindowShouldClose(m_window);
    }

    GLFWwindow* Window::getMWindow() const {
        return m_window;
    }

    int Window::getHeight() const {
        return m_height;
    }

    int Window::getWidth() const {
        return m_width;
    }

    void Window::windowResize(GLFWwindow* window, int width, int height) {
        glViewport(0, 0, width, height);
    }
}
