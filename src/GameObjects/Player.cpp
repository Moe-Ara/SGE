//
// Created by Mohamad on 12/06/2024.
//

#include "Player.h"
#include <iostream>
#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/vec3.hpp>

namespace SGE::actors {

    Player::~Player() {}

    Transform Player::getTransform() {
        return transform;
    }

    void Player::jump(float deltaTime) {
        transform.translation.y += jumpSpeed * deltaTime;
        // Introduce a way to make a delay or reverse the jump
        transform.translation.y -= jumpSpeed * deltaTime;
    }

    void Player::move(glm::vec3 movement, float deltaTime) {
        transform.translation += movement * deltaTime;
    }

    void Player::fire(float deltaTime) {
        // Implement fire logic if needed
    }

    void Player::dash(float deltaTime) {
        // Implement dash logic if needed
    }

    void Player::update(float deltaTime) {
        glm::vec3 movement{0, 0, 0};

        if (inputHandler.isKeyPressed(GLFW_KEY_SPACE)) {
            std::cout << "Jumped" << std::endl;
            jump(deltaTime);
        }

        if (inputHandler.isKeyPressed(GLFW_KEY_W)) {
            movement += camera.getForward() * movementSpeed * deltaTime;
        }

        if (inputHandler.isKeyPressed(GLFW_KEY_S)) {
            movement -= camera.getForward() * movementSpeed * deltaTime;
        }

        if (inputHandler.isKeyPressed(GLFW_KEY_A)) {
            movement -= camera.getRight() * movementSpeed * deltaTime;
        }

        if (inputHandler.isKeyPressed(GLFW_KEY_D)) {
            movement += camera.getRight() * movementSpeed * deltaTime;
        }

        // Handle mouse movement
        handleMouseMovement();

        // Update camera position and direction
        glm::vec3 cameraPos = transform.translation + cameraOffset;
        camera.setViewDirection(transform.translation + cameraOffset, calculateDirection(yaw, pitch), glm::vec3(0.0f, 1.0f, 0.0f));

        move(movement, deltaTime);
        render();
    }

    void Player::handleMouseMovement() {
        double mouseX, mouseY;
        glfwGetCursorPos(graphics::Window::getWindow()->getMWindow(), &mouseX, &mouseY);

        // Calculate delta movement
        double deltaX = mouseX - lastMouseX;
        double deltaY = lastMouseY - mouseY; // Invert mouse Y axis

        const float sensitivity = 0.1f;
        yaw += deltaX * sensitivity;
        pitch -= deltaY * sensitivity;
        pitch = glm::clamp(pitch, -89.0f, 89.0f);  // Limit pitch angle

        // Reset mouse position to center
        glfwSetCursorPos(graphics::Window::getWindow()->getMWindow(), lastMouseX, lastMouseY);
    }

    float Player::getMovementSpeed() {
        return movementSpeed;
    }

    void Player::setMovementSpeed(float speed) {
        movementSpeed = speed;
    }

    float Player::getJumpSpeed() {
        return jumpSpeed;
    }

    void Player::setJumpSpeed(float speed) {
        jumpSpeed = speed;
    }

    Player::Player() {
        camera.setPrespectiveProjection(glm::radians(45.0f), 16.0f / 9.0f, 0.1f, 100.0f);
        camera.setViewDirection(cameraPosition, glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        modelPtr = SGE::actors::Model::createModelFromFile("cube.obj");

        // Initialize mouse position and hide cursor
        lastMouseX = graphics::Window::getWindow()->getWidth() / 2;  // Assuming window width is 800
        lastMouseY = graphics::Window::getWindow()->getHeight() / 2; // Assuming window height is 600
        glfwSetCursorPos(graphics::Window::getWindow()->getMWindow(), lastMouseX, lastMouseY);
        glfwSetInputMode(graphics::Window::getWindow()->getMWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        // Initialize tracking camera offset
        cameraOffset = glm::vec3(10, -5, -5);
    }

    void Player::render() {
        shader.enable();
        shader.setUniformMat4("transform", transform.mat4());
        shader.setUniformMat4("projection", camera.getProjection());
        shader.setUniformMat4("view", camera.getView());
        shader.setUniformFloat3("color", glm::vec3(1.0f, 0.0f, 1.0f));

        modelPtr->bind();
        std::cout << "Rendering model" << std::endl;
        modelPtr->render();
        modelPtr->unbind();
    }

    glm::vec3 Player::calculateDirection(double yaw, double pitch) {
        return {
                cos(glm::radians(yaw)) * cos(glm::radians(pitch)),
                sin(glm::radians(pitch)),
                sin(glm::radians(yaw)) * cos(glm::radians(pitch))
        };
    }

}
