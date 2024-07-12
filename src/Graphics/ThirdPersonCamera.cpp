//
// Created by Mohamad on 30/06/2024.
//

#include <glm/ext/matrix_transform.hpp>
#include "ThirdPersonCamera.h"
#include "Window.h"

namespace SGE::GRAPHICS {
    ThirdPersonCamera::ThirdPersonCamera(glm::vec3 offset)
            : m_cameraOffset(offset), m_yaw(0.f), m_pitch(0.f), Camera() {
        setPrespectiveProjection(glm::radians(45.0f), 16.0f / 9.0f, 0.1f, 100.0f);
    }

    void ThirdPersonCamera::update(float deltaTime,std::shared_ptr<SGE::GAMEOBJECTS::Actor> target) {

        if (target) {
            glm::vec3 targetPosition = target->getTransform().getMTranslation();
            glm::vec3 cameraPosition = targetPosition + m_cameraOffset;
            handleMouseMovement(target);
        }
    }

    void ThirdPersonCamera::handleMouseMovement(std::shared_ptr<SGE::GAMEOBJECTS::Actor> target) {
        double mouseX, mouseY;
        glfwGetCursorPos(GRAPHICS::Window::getInstance().getMWindow(), &mouseX, &mouseY);

        // Calculate delta m_movement
        double deltaX = mouseX - m_lastMouseX;
        double deltaY = m_lastMouseY - mouseY; // Invert mouse Y axis

        const float sensitivity = 0.05f;
        m_yaw -= deltaX * sensitivity;
        m_pitch += deltaY * sensitivity;
        m_pitch = glm::clamp(m_pitch, -89.0f, 89.0f);  // Limit m_pitch angle

        // Calculate the new camera position based on m_yaw and m_pitch
        float radius = glm::length(m_cameraOffset);
        glm::vec3 newCameraPos;
        newCameraPos.x = target->getTransform().getMTranslation().x + radius * cos(glm::radians(m_pitch)) * sin(glm::radians(m_yaw));
        newCameraPos.y = target->getTransform().getMTranslation().y + radius * sin(glm::radians(m_pitch));
        newCameraPos.z = target->getTransform().getMTranslation().z + radius * cos(glm::radians(m_pitch)) * cos(glm::radians(m_yaw));

        // Update the camera to look at the player
        setViewTarget(newCameraPos, target->getTransform().getMTranslation(), glm::vec3(0.0f, 1.0f, 0.0f));

        // Reset mouse position to center
        m_lastMouseX = GRAPHICS::Window::getInstance().getWidth() / 2;
        m_lastMouseY = GRAPHICS::Window::getInstance().getHeight() / 2;
        glfwSetCursorPos(GRAPHICS::Window::getInstance().getMWindow(), m_lastMouseX, m_lastMouseY);
    }

    float ThirdPersonCamera::getYaw() {
        return m_yaw;
    }
} // GRAPHICS