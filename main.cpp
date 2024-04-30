#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "src/sge_object.h"
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "src/sge_model.h"
#include "src/sge_window.h"
#include "src/sge_input_handler.h"
#include "src/sge_shader.h"
#include "src/sge_camera.h"

void errorCallback(int error, const char *description) {
    std::cerr << "Error: " << description << std::endl;
}

int main() {
    // Set GLFW error callback
    glfwSetErrorCallback(errorCallback);
    SGE::graphics::sge_window window("SGE", 960, 540);
    SGE::utils::sge_input_handler::setupKeyHandler(&window);

    SGE::utils::sge_input_handler inputHandler({GLFW_KEY_A,
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

//    GLfloat vertices[] = {
//            // Positions
//            -0.5f,  -0.5f, 0.0f,   // Top left
//            -0.5f,  0.5f, 0.0f,   // Top left
//            0.5f,  0.5f, 0.0f,    // Top right
//            0.5f,  0.5f, 0.0f,    // Top right
//            0.5f,  -0.5f, 0.0f,    // Top right
//            -0.5f,  -0.5f, 0.0f    // Top right
//    };
//    GLuint indices[] = {
//            0, 1, 2,  // First triangle
//            0, 2, 3   // Second triangle
//    };
    auto model = SGE::actors::sge_model::createModelFromFile("cube.obj");
    model->bind();
//    glGenBuffers(1,&vbo);
//    glBindBuffer(GL_ARRAY_BUFFER,vbo);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
//    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE, 0,0 );
//    glEnableVertexAttribArray(0);

    SGE::graphics::sge_camera camera{};
    glm::mat4 ortho;
    ortho = glm::ortho(-14.0f, 16.0f, -10.0f, 12.0f, -12.0f, 12.0f);
    SGE::graphics::sge_shader shader("vertex.vert", "fragment.frag");
    shader.enable();
//    shader.setUniformMat4("projection",ortho);
//    shader.setUniformMat4("view",ortho);
    camera.setPrespectiveProjection(glm::radians(45.0f), 16.0f / 9.0f, 0.1f, 100.0f);

    // Set initial camera position
    glm::vec3 cameraPosition(0.0f, 0.0f, 10.0f);
    camera.setViewDirection(cameraPosition, glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));

// Define the rotation angles (in radians) for yaw, pitch, and roll
    float yaw = glm::radians(1.0f);    // Example yaw angle
    float pitch = glm::radians(1.0f);  // Example pitch angle
    float roll = glm::radians(1.0f);    // Example roll angle

// Set the camera's view using the setViewYXZ() function
    // Access matrices if needed
    glm::mat4 projectionMatrix = camera.getProjection();
    glm::mat4 viewMatrix = camera.getView();
    // Extract the forward vector from the view matrix
    glm::vec3 forward = -glm::vec3(viewMatrix[0][2], viewMatrix[1][2], viewMatrix[2][2]);

// Calculate yaw angle (rotation around the y-axis)
    float originalYaw = glm::degrees(atan2(forward.x, forward.z));

// Calculate pitch angle (rotation around the x-axis)
    float originalPitch = glm::degrees(asin(forward.y));
    while (!window.closed()) {

//
//        shader.setUniformMat4("transform",
//                              glm::rotate(glm::mat4(1.0f), glm::radians(45.0f+x), glm::vec3(0.0f, 1.0f, 0.f)));
//        x += 0.01f;


        shader.setUniformMat4("projection", camera.getProjection());
        shader.setUniformMat4("view", camera.getView());

        shader.setUniformFloat3("color", glm::vec3(1.0f, 0.f, 1.f));

        // Check for movement input
        if (inputHandler.isKeyPressed(GLFW_KEY_W)) {
            cameraPosition += glm::vec3(camera.getView()[2]) * 0.1f; // Move camera forward along its view direction
        }
        if (inputHandler.isKeyPressed(GLFW_KEY_S)) {
            cameraPosition -= glm::vec3(camera.getView()[2]) * 0.1f; // Move camera backward along its view direction
        }
        if (inputHandler.isKeyPressed(GLFW_KEY_A)) {
            cameraPosition -= glm::vec3(camera.getView()[0]) * 0.1f; // Move camera to the left (negative right vector)
        }
        if (inputHandler.isKeyPressed(GLFW_KEY_D)) {
            cameraPosition += glm::vec3(camera.getView()[0]) * 0.1f; // Move camera to the right (positive right vector)
        }
        if (inputHandler.isKeyPressed(GLFW_KEY_E)) {
            cameraPosition += glm::vec3(camera.getView()[1]) * 0.1f; // Move camera up along its up vector
        }
        if (inputHandler.isKeyPressed(GLFW_KEY_Q)) {
            cameraPosition -= glm::vec3(camera.getView()[1]) * 0.1f; // Move camera down along its up vector
        }
        if (inputHandler.isMouseButtonPressed(1)) {

            yaw = glm::radians(originalYaw) + SGE::utils::sge_input_handler::m_mouseY / 100 * 0.1f;
            pitch = glm::radians(originalPitch) + SGE::utils::sge_input_handler::m_mouseX / 100 * 0.1f;
//            pitch =glm::clamp(pitch, -glm::half_pi<float>(), glm::half_pi<float>());
        }
        // Update camera position
        camera.setViewDirection(cameraPosition, glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        camera.setViewYXZ(cameraPosition, glm::vec3(yaw, pitch, roll));

        window.clear();
        model->render();

//        glDrawArrays(GL_TRIANGLES,0,6);
        window.update();


    }

    return 0;
}
