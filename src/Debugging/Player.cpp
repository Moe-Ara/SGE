//
// Created by Mohamad on 12/06/2024.
//

#include "Player.h"

namespace SGE::DEBUGGING {
    Player::Player(long id, SGE::GRAPHICS::Camera &thirdPersonCamera)
            : SGE::GAMEOBJECTS::Actor(id, SGE::GAMEOBJECTS::Transform{},
                                     SGE::UTILS::ModelLoader::loadModelFromFile("cube.obj")),
              m_thirdPersonCamera(thirdPersonCamera) {
        // The model is already set in the base class constructor
        // No need to manually assign it here
    }

    void Player::jump(float deltaTime) {
        auto translation = glm::vec3(0, jumpSpeed, 0) * deltaTime;
        getTransformRef().translate(translation);
    }

    void Player::move(glm::vec3 movement, float deltaTime) {
        getTransformRef().translate(movement * deltaTime);
    }

    void Player::fire(float deltaTime) {
        // Implementation for firing
    }

    void Player::dash(float deltaTime) {
        auto translation = glm::vec3(0, jumpSpeed, 0) * deltaTime;
        getTransformRef().translate(translation);
    }

    void Player::update(float deltaTime) {
        Actor::update(deltaTime);
        handleMovement(deltaTime);
        move(m_movement, deltaTime);
        m_movement = glm::vec3(0.0f); // Reset to zero vector instead of *=0
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

    glm::vec3 Player::getColor() const {
        return {2.f, 0.f, 0.f};
    }

    void Player::render(const std::shared_ptr<SGE::GRAPHICS::Shader>& shader) {
        (void)shader;
    }

    void Player::handleMovement(float deltaTime) {
        if (inputHandler.isKeyPressed(GLFW_KEY_SPACE)) {
            jump(deltaTime);
        }
        if (inputHandler.isKeyPressed(GLFW_KEY_LEFT_SHIFT)) {
            dash(deltaTime);
        }

        glm::vec3 forward = m_thirdPersonCamera.getForward();
        forward.y = 0.0f;
        forward = glm::normalize(forward);
        glm::vec3 right = m_thirdPersonCamera.getRight();
        right.y = 0.0f;
        right = glm::normalize(right);

        if (inputHandler.isKeyPressed(GLFW_KEY_W)) {
            m_movement += forward * movementSpeed * deltaTime;
        }
        if (inputHandler.isKeyPressed(GLFW_KEY_S)) {
            m_movement -= forward * movementSpeed * deltaTime;
        }
        if (inputHandler.isKeyPressed(GLFW_KEY_A)) {
            m_movement -= right * movementSpeed * deltaTime;
        }
        if (inputHandler.isKeyPressed(GLFW_KEY_D)) {
            m_movement += right * movementSpeed * deltaTime;
        }
    }
}

