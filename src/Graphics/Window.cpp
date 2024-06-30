#include <iostream>
#include "Window.h"

namespace SGE::graphics {

    std::unique_ptr<Window> Window::s_window = nullptr;
    void windowResize(GLFWwindow *window, int width, int height) {
        glViewport(0, 0, width, height);
    }

    Window::Window(const char *name, int width, int height)
            : m_title(name), m_width(width), m_height(height) {
        init();
        s_window=std::unique_ptr<Window>(this);
    }

    Window::~Window() {
        glfwDestroyWindow(m_window); // Ensure proper cleanup
        glfwTerminate();
    }

    void Window::update() {
        glfwPollEvents();
        glfwSwapBuffers(m_window);
    }

    void Window::init() {
        if (!glfwInit()) {
            std::cerr << "ERROR: glfwInit FAILED" << std::endl;
            return;
        }
        m_window = glfwCreateWindow(m_width, m_height, m_title, NULL, NULL);
        if (!m_window) {
            std::cerr << "ERROR: Failed To Create Window" << std::endl;
            glfwTerminate(); // Call glfwTerminate if Window creation fails
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

    bool Window::closed() const {
        return glfwWindowShouldClose(m_window);
    }

    void Window::clear() const {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    GLFWwindow *Window::getMWindow() const {
        return m_window;
    }

    Window *Window::getWindow() {
        return s_window.get();
    }

    int Window::getHeight() {
        return m_height;
    }

    int Window::getWidth() {
        return m_width;
    }

}
