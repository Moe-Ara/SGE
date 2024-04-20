//
// Created by Mohamad on 20/04/2024.
//

#ifndef GLCPP_GAME_WINDOW_H
#define GLCPP_GAME_WINDOW_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace glcpp {
    class game_window {
    public:
        game_window(const char *name, int width, int height);

        ~game_window();

        void update();

        void clear() const;

        bool closed() const;

    private:
        const char *m_title;
        int m_width, m_height;
        GLFWwindow *m_window;
    public:
        GLFWwindow *getMWindow() const;

    private:
        void init();


    };
}


#endif //GLCPP_GAME_WINDOW_H
