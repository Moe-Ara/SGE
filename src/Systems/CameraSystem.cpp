#include "CameraSystem.h"
#include "../ECS/Components.h"
#include "../Graphics/Window.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/trigonometric.hpp>
#include <cmath>

namespace SGE::SYSTEMS {

    CameraSystem::CameraSystem(GRAPHICS::Window& window) : window(window) {}

    void CameraSystem::update(entt::registry& registry, float deltaTime) {
        (void)deltaTime;

        if (window.isHeadless()) {
            return;
        }

        const float aspect = window.getHeight() > 0
            ? static_cast<float>(window.getWidth()) / static_cast<float>(window.getHeight())
            : 16.0f / 9.0f;

        auto cameraView = registry.view<ECS::CameraComponent>();
        for (auto entity : cameraView) {
            auto& camComp = cameraView.get<ECS::CameraComponent>(entity);
            camComp.camera.setPrespectiveProjection(camComp.fovYRadians, aspect, camComp.nearPlane, camComp.farPlane);
        }

        auto view = registry.view<ECS::CameraComponent, ECS::ThirdPersonFollowComponent>();
        for (auto entity : view) {
            auto& camComp = view.get<ECS::CameraComponent>(entity);
            auto& follow = view.get<ECS::ThirdPersonFollowComponent>(entity);

            if (follow.target == entt::null || !registry.valid(follow.target) ||
                !registry.all_of<ECS::TransformComponent>(follow.target)) {
                continue;
            }
            const auto& targetTransform = registry.get<ECS::TransformComponent>(follow.target);

            // Only steer the camera (and capture the cursor) while the right mouse
            // button is held, so the mouse stays free for the ImGui editor otherwise.
            const bool rotating = glfwGetMouseButton(window.getMWindow(), GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;
            glfwSetInputMode(window.getMWindow(), GLFW_CURSOR, rotating ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);

            if (rotating) {
                double mouseX = follow.lastMouseX;
                double mouseY = follow.lastMouseY;
                glfwGetCursorPos(window.getMWindow(), &mouseX, &mouseY);

                if (!follow.initialized) {
                    follow.lastMouseX = mouseX;
                    follow.lastMouseY = mouseY;
                    follow.initialized = true;
                }

                double deltaX = mouseX - follow.lastMouseX;
                double deltaY = follow.lastMouseY - mouseY; // Invert mouse Y axis

                const float sensitivity = 0.05f;
                follow.yaw -= static_cast<float>(deltaX) * sensitivity;
                follow.pitch += static_cast<float>(deltaY) * sensitivity;
                follow.pitch = glm::clamp(follow.pitch, -89.0f, 89.0f);

                follow.lastMouseX = window.getWidth() / 2.0;
                follow.lastMouseY = window.getHeight() / 2.0;
                glfwSetCursorPos(window.getMWindow(), follow.lastMouseX, follow.lastMouseY);
            } else {
                follow.initialized = false;
            }

            const float radius = glm::length(follow.offset);
            glm::vec3 newCameraPos;
            newCameraPos.x = targetTransform.translation.x + radius * std::cos(glm::radians(follow.pitch)) * std::sin(glm::radians(follow.yaw));
            newCameraPos.y = targetTransform.translation.y + radius * std::sin(glm::radians(follow.pitch));
            newCameraPos.z = targetTransform.translation.z + radius * std::cos(glm::radians(follow.pitch)) * std::cos(glm::radians(follow.yaw));

            camComp.camera.setViewTarget(newCameraPos, targetTransform.translation, glm::vec3(0.0f, 1.0f, 0.0f));
        }
    }

} // namespace SGE::SYSTEMS
