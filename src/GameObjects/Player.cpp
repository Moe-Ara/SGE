#include "Player.h"
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/vec3.hpp>

namespace SGE::actors {

    Player::~Player() {}

    Transform Player::getTransform() {
        return this->transform;
    }

    void Player::jump(float deltaTime) {
        transform.translation.y -= jumpSpeed * deltaTime;
        // Placeholder for actual jump logic
    }

    void Player::move(glm::vec3 movement, float deltaTime) {
        transform.translation += movement * deltaTime;  // Update position based on movement
    }

    void Player::fire(float deltaTime) {
        // Fire logic can be implemented here
    }

    void Player::dash(float deltaTime) {
        transform.translation.y += jumpSpeed * deltaTime;

        // Dash logic can be implemented here
    }

    void Player::update(float deltaTime) {
        glm::vec3 movement{0, 0, 0};

        if (inputHandler.isKeyPressed(GLFW_KEY_SPACE)) {
            jump(deltaTime);
        }
        if (inputHandler.isKeyPressed(GLFW_KEY_LEFT_SHIFT)){
            dash(deltaTime);
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
        //rotate player to face the direction the camera is facing
        transform.rotation = glm::quat(glm::vec3(0.0f, glm::radians(yaw), 0.0f));

        // Handle mouse movement
        handleMouseMovement();

        // Update camera position and direction
        glm::vec3 cameraPos = transform.translation + cameraOffset;
        move(movement, deltaTime);


    }

    void Player::handleMouseMovement() {
        double mouseX, mouseY;
        glfwGetCursorPos(graphics::Window::getWindow()->getMWindow(), &mouseX, &mouseY);

        // Calculate delta movement
        double deltaX = mouseX - lastMouseX;
        double deltaY = lastMouseY - mouseY; // Invert mouse Y axis

        const float sensitivity = 0.05f;
        yaw -= deltaX * sensitivity;
        pitch += deltaY * sensitivity;
        pitch = glm::clamp(pitch, -89.0f, 89.0f);  // Limit pitch angle

        // Calculate the new camera position based on yaw and pitch
        float radius = glm::length(cameraOffset);
        glm::vec3 newCameraPos;
        newCameraPos.x = transform.translation.x + radius * cos(glm::radians(pitch)) * sin(glm::radians(yaw));
        newCameraPos.y = transform.translation.y + radius * sin(glm::radians(pitch));
        newCameraPos.z = transform.translation.z + radius * cos(glm::radians(pitch)) * cos(glm::radians(yaw));

        // Update the camera to look at the player
        camera.setViewTarget(newCameraPos, transform.translation, glm::vec3(0.0f, 1.0f, 0.0f));

        // Reset mouse position to center
        lastMouseX = graphics::Window::getWindow()->getWidth() / 2;
        lastMouseY = graphics::Window::getWindow()->getHeight() / 2;
        glfwSetCursorPos(graphics::Window::getWindow()->getMWindow(), lastMouseX, lastMouseY);
    }

    glm::vec3 Player::calculateDirection(float yaw, float pitch) {
        glm::vec3 direction;
        direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        direction.y = sin(glm::radians(pitch));
        direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        return glm::normalize(direction);
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

    Player::Player() : yaw(0.0f), pitch(0.0f) {
        camera.setPrespectiveProjection(glm::radians(45.0f), 16.0f / 9.0f, 0.1f, 100.0f);
        camera.setViewDirection(cameraPosition, transform.translation, glm::vec3(0.0f, 1.0f, 0.0f));
        modelPtr = SGE::actors::Model::createModelFromFile("cube.obj");

        // Initialize mouse position and hide cursor
        lastMouseX = graphics::Window::getWindow()->getWidth() / 2;  // Assuming window width is 800
        lastMouseY = graphics::Window::getWindow()->getHeight() / 2; // Assuming window height is 600
//        glfwSetCursorPos(graphics::Window::getWindow()->getMWindow(), lastMouseX, lastMouseY);
//        glfwSetInputMode(graphics::Window::getWindow()->getMWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        // Initialize tracking camera offset
        cameraOffset = glm::vec3(10, -2, 0);
    }

    void Player::render() {


        modelPtr->bind();
        modelPtr->render();
        modelPtr->unbind();
    }

    SGE::graphics::Camera& Player::getCamera() {
        return camera;
    }

    glm::vec3 Player::getColor() {
        return {2.f,0.f,0.f};
    }
}
