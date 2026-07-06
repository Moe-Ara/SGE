#include "Window.h"
#include "../Exceptions/SGE_Exception.h"
#include <cstdlib>
#include <iostream>

namespace SGE::GRAPHICS {

    Window& Window::getInstance(const std::string& title, int width, int height) {
        static Window instance(title, width, height);
        return instance;
    }

    Window::Window(const std::string& title, int width, int height)
            : m_title(title), m_width(width), m_height(height), m_window(nullptr), m_headless(false) {
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
            const char* display = std::getenv("DISPLAY");
            const char* waylandDisplay = std::getenv("WAYLAND_DISPLAY");
            if ((display == nullptr || display[0] == '\0') && (waylandDisplay == nullptr || waylandDisplay[0] == '\0')) {
                m_headless = true;
                glfwTerminate();
                std::cerr << "Headless environment detected; running without a GLFW window." << std::endl;
                return;
            }
            throw SGE::EXCEPTIONS::SGE_Exception("Failed to initialize GLFW");
        }

        m_window = glfwCreateWindow(m_width, m_height, m_title.c_str(), nullptr, nullptr);
        if (!m_window) {
            const char* display = std::getenv("DISPLAY");
            const char* waylandDisplay = std::getenv("WAYLAND_DISPLAY");
            if ((display == nullptr || display[0] == '\0') && (waylandDisplay == nullptr || waylandDisplay[0] == '\0')) {
                m_headless = true;
                glfwTerminate();
                std::cerr << "Headless environment detected; running without a GLFW window." << std::endl;
                return;
            }
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
        if (m_headless || !m_window) {
            return;
        }
        glfwPollEvents();
        glfwSwapBuffers(m_window);
    }

    void Window::clear() const {
        if (m_headless || !m_window) {
            return;
        }
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    bool Window::closed() const {
        if (m_headless || !m_window) {
            return true;
        }
        return glfwWindowShouldClose(m_window);
    }

    bool Window::isHeadless() const {
        return m_headless;
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
