#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "src/game_object.h"
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "src/game_model.h"
#include "src/game_window.h"
#include "src/game_input_handler.h"
#include "src/game_shader.h"
void errorCallback(int error, const char *description) {
    std::cerr << "Error: " << description << std::endl;
}

int main() {
    // Set GLFW error callback
    glfwSetErrorCallback(errorCallback);
    glcpp::game_window window("SGE", 960, 540);
    glcpp::game_input_handler::setupKeyHandler(&window);

    glcpp::game_input_handler inputHandler({GLFW_KEY_A,
                                            GLFW_KEY_S,
                                            GLFW_KEY_W,
                                            GLFW_KEY_D,
                                            GLFW_KEY_LEFT_CONTROL,
                                            GLFW_KEY_LEFT_SHIFT,
                                            GLFW_KEY_Q,
                                            GLFW_KEY_E},
                                           {GLFW_MOUSE_BUTTON_RIGHT, GLFW_MOUSE_BUTTON_LEFT});


    glClearColor(0.f, 0.2f, 0.3f, 1.0f);
    float x = 0.1;

    GLfloat vertices[] = {
            // Positions
            -0.5f,  -0.5f, 0.0f,   // Top left
            -0.5f,  0.5f, 0.0f,   // Top left
            0.5f,  0.5f, 0.0f,    // Top right
            0.5f,  0.5f, 0.0f,    // Top right
            0.5f,  -0.5f, 0.0f,    // Top right
            -0.5f,  -0.5f, 0.0f    // Top right
    };
    GLuint indices[] = {
            0, 1, 2,  // First triangle
            0, 2, 3   // Second triangle
    };
    auto model=glcpp::game_model::createModelFromFile("flat_vase.obj");
    model->bind();
    GLuint vbo;
//    glGenBuffers(1,&vbo);
//    glBindBuffer(GL_ARRAY_BUFFER,vbo);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
//    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE, 0,0 );
//    glEnableVertexAttribArray(0);

//    glm::mat4 ortho;
//    ortho = glm::ortho(0.0f, 9.0f, 0.0f, 9.0f, -1.0f, 1.0f);
    glcpp::graphics::game_shader shader("vertex.vert","fragment.frag");
    shader.enable();
//    shader.setUniformMat4("projection",ortho);
//    shader.setUniformMat4("view",ortho);

    while (!window.closed()) {
//        shader.setUniformMat4("model",glm::rotate(glm::mat4(1.0f), glm::radians(45.0f*x), glm::vec3(1.0f, 0.0f, 0.f)));
        shader.setUniformFloat3("color",glm::vec3(1.0f,0.f,1.f));
        x+=0.01f;
        window.clear();
        model->render();

//        glDrawArrays(GL_TRIANGLES,0,6);
        window.update();
//        if (inputHandler.isKeyPressed(GLFW_KEY_A)) {
//
//            std::cout << "PRESSED" << std::endl;
//
//        }
//        if (inputHandler.isMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT)) {
//            std::cout << "Button Works" << " Mouse At:"<<inputHandler.m_mouseX<<","<<inputHandler.m_mouseY<<std::endl;
//        }


    }

    return 0;
}
