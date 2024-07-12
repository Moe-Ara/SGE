#ifndef GLCPP_WINDOW_H
#define GLCPP_WINDOW_H

#include <gl/glew.h>
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
        void clear() const;
        bool closed() const;
        GLFWwindow* getMWindow() const;
        int getHeight() const;
        int getWidth() const;

    private:
        // Private constructor
        Window(const std::string& title, int width, int height);
        ~Window();

        // Initialization method
        void init();

        std::string m_title;
        int m_width, m_height;
        GLFWwindow* m_window;

        static void windowResize(GLFWwindow* window, int width, int height);
    };
}

#endif //GLCPP_WINDOW_H
