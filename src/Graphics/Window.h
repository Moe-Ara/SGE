#ifndef GLCPP_WINDOW_H
#define GLCPP_WINDOW_H

#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include <memory>

namespace SGE::graphics {

    class Window {
    public:
        Window(const char *name, int width, int height);
        ~Window();

        void update();
        void clear() const;
        bool closed() const;
        static Window* getWindow(); // Static method to get the window instance
        GLFWwindow *getMWindow() const;
        int getHeight();
        int getWidth();

    private:
        const char *m_title;
        int m_width, m_height;
        GLFWwindow *m_window;

        void init();
        static std::unique_ptr<Window> s_window; // Unique pointer for window instance
    };

}

#endif //GLCPP_WINDOW_H
