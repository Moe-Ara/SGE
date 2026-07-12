#ifndef GLCPP_WINDOW_H
#define GLCPP_WINDOW_H

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <memory>
#include <string>

namespace SGE::GRAPHICS {

    class Window {
    public:
        // Get the singleton instance
        static Window& getInstance(const std::string& title = "SGE Window", int width = 800, int height = 600);

        // Deleted copy constructor and assignment operator
        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;

        // Methods to interact with the window
        void update();
        void pollEvents() const;
        void present() const;
        void clear() const;
        bool closed() const;
        bool isHeadless() const;
        GLFWwindow* getMWindow() const;
        int getHeight() const;
        int getWidth() const;
        int getFramebufferHeight() const;
        int getFramebufferWidth() const;

    private:
        // Private constructor
        Window(const std::string& title, int width, int height);
        ~Window();

        // Initialization method
        void init();

        std::string m_title;
        int m_width, m_height;
        int m_framebufferWidth, m_framebufferHeight;
        GLFWwindow* m_window;
        bool m_headless;

        static void windowResize(GLFWwindow* window, int width, int height);
        static void framebufferResize(GLFWwindow* window, int width, int height);
    };
}

#endif //GLCPP_WINDOW_H
