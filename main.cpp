#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "src/game_object.h"
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "src/game_model.h"
#include "src/game_window.h"
void errorCallback(int error, const char* description) {
    std::cerr << "Error: " << description << std::endl;
}

int main() {
    // Set GLFW error callback
    glfwSetErrorCallback(errorCallback);

    glcpp::graphics::game_window window("My Window", 960,540);

//    std::shared_ptr<glcpp::game_model> model=glcpp::game_model::createModelFromFile("smooth_vase.obj");

//    while (!glfwWindowShouldClose(window)) {
//        // Set up OpenGL rendering states
//        glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Clear color: black
//        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear color and depth buffer
//
//// Enable depth testing for proper rendering of 3D objects
//        glEnable(GL_DEPTH_TEST);
//
//        model->render();
////// Bind the vertex array object (VAO)
////        glBindVertexArray(model->getVao());
////
////// Draw the model using indexed rendering
////        glDrawElements(GL_TRIANGLES, model->getIndexCount(), GL_UNSIGNED_INT, 0);
////
////// Unbind the vertex array object (VAO)
////        glBindVertexArray(0);
//
//// Swap buffers and poll events
//        glfwSwapBuffers(window);
//        glfwPollEvents();
//    }
//

    glClearColor(0.f,0.2f,0.3f,1.0f);
    while(!window.closed()){
        window.clear();
        glBegin(GL_QUADS);
        glVertex2f(-0.5f,-0.5f);
        glVertex2f(-0.5f,0.5f);
        glVertex2f(0.5f,0.5f);
        glVertex2f(0.5f,-0.5f);

        glEnd();
        window.update();
    }

    return 0;
}
