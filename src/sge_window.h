#ifndef GLCPP_SGE_WINDOW_H
#define GLCPP_SGE_WINDOW_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace SGE::graphics {

    class sge_window {
    public:
        sge_window(const char *name, int width, int height);
        ~sge_window();

        void update();
        void clear() const;
        bool closed() const;

        GLFWwindow *getMWindow() const;

    private:
        const char *m_title;
        int m_width, m_height;
        GLFWwindow *m_window;

        void init();
    };

}

#endif //GLCPP_SGE_WINDOW_H
