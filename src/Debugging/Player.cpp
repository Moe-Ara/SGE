#include "Player.h"
#include <iostream>
#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/vec3.hpp>
#include "../Utils/ModelLoader.h"

namespace SGE::DEBUGGING {

    Player::~Player() {}


    void Player::jump(float deltaTime) {
        auto translation = glm::vec3(0, -jumpSpeed, 0) * deltaTime;
        transform.translate(translation);

    }

    void Player::move(glm::vec3 movement, float deltaTime) {
        transform.translate(movement * deltaTime);  // Update position based on m_movement

    }

    void Player::fire(float deltaTime) {
    }

    void Player::dash(float deltaTime) {
        auto translation = glm::vec3(0, jumpSpeed, 0) * deltaTime;
        transform.translate(translation);
    }

    void Player::update(float deltaTime) {
        Actor::update(deltaTime);
        handleMovement(deltaTime);
        move(m_movement, deltaTime);
        m_movement*=0;

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

    Player::Player(long id, SGE::GRAPHICS::Camera &thirdPersonCamera) : m_thirdPersonCamera(
            thirdPersonCamera), Actor(id, SGE::GAMEOBJECTS::Transform{},
                                      SGE::UTILS::ModelLoader::loadModelFromFile("cube.obj")) {
        modelPtr=GAMEOBJECTS::GameObject::model;
    }


    glm::vec3 Player::getColor() const {
        return {2.f, 0.f, 0.f};
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
            m_movement -= right* movementSpeed * deltaTime;
        }
        if (inputHandler.isKeyPressed(GLFW_KEY_D)) {
            m_movement += right * movementSpeed * deltaTime;
        }
        auto rotation=glm::quat(glm::vec3(0.0f, glm::radians(m_thirdPersonCamera.getYaw()), 0.0f));
        transform.rotate(rotation);

    }
}
