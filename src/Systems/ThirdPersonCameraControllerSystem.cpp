#include "ThirdPersonCameraControllerSystem.h"
#include "../ECS/Components.h"
#include "../Graphics/Window.h"
#include "../Utils/CameraMath.h"

#include <GLFW/glfw3.h>
#include <glm/common.hpp>
#include <glm/geometric.hpp>
#include <glm/trigonometric.hpp>
#include <cmath>

namespace SGE::SYSTEMS {

    ThirdPersonCameraControllerSystem::ThirdPersonCameraControllerSystem(
        GRAPHICS::Window& window,
        std::shared_ptr<INPUT::InputHandler> inputHandler
    ) : window(window), inputHandler(std::move(inputHandler)) {}

    void ThirdPersonCameraControllerSystem::update(entt::registry& registry, float deltaTime) {
        (void)deltaTime;

        if (window.isHeadless() || !inputHandler) {
            return;
        }

        auto view = registry.view<
            ECS::CameraComponent,
            ECS::TransformComponent,
            ECS::ThirdPersonFollowComponent
        >();

        for (auto entity : view) {
            auto& transform = view.get<ECS::TransformComponent>(entity);
            auto& follow = view.get<ECS::ThirdPersonFollowComponent>(entity);

            if (!follow.enabled) {
                continue;
            }

            if (follow.target == entt::null ||
                !registry.valid(follow.target) ||
                !registry.all_of<ECS::TransformComponent>(follow.target)) {
                continue;
            }

            const auto& targetTransform =
                registry.get<ECS::TransformComponent>(follow.target);

            if (!follow.orbitInitialized) {
                const float radius = glm::length(follow.offset);
                if (radius > 0.0001f) {
                    follow.yaw = glm::degrees(glm::atan(follow.offset.x, follow.offset.z));
                    follow.pitch = glm::degrees(glm::asin(
                        glm::clamp(follow.offset.y / radius, -1.0f, 1.0f)
                    ));
                }
                follow.orbitInitialized = true;
            }

            const bool rightMouseDown =
                inputHandler->isMouseButtonPressedRaw(GLFW_MOUSE_BUTTON_RIGHT);
            if (!rightMouseDown) {
                follow.mouseLookActive = false;
            } else if (!follow.mouseLookActive && !inputHandler->isMouseCaptured()) {
                follow.mouseLookActive = true;
            }
            const bool rotating = follow.mouseLookActive;

            glfwSetInputMode(
                window.getMWindow(),
                GLFW_CURSOR,
                rotating ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL
            );

            if (rotating) {
                const double mouseX = INPUT::InputHandler::getMouseX();
                const double mouseY = INPUT::InputHandler::getMouseY();

                if (!follow.initialized) {
                    follow.lastMouseX = mouseX;
                    follow.lastMouseY = mouseY;
                    follow.initialized = true;
                }

                const double deltaX = mouseX - follow.lastMouseX;
                const double deltaY = follow.lastMouseY - mouseY;

                follow.yaw -= static_cast<float>(deltaX) * follow.mouseSensitivity;
                follow.pitch = UTILS::clampPitch(
                    follow.pitch + static_cast<float>(deltaY) * follow.mouseSensitivity
                );

                follow.lastMouseX = window.getWidth() / 2.0;
                follow.lastMouseY = window.getHeight() / 2.0;

                glfwSetCursorPos(
                    window.getMWindow(),
                    follow.lastMouseX,
                    follow.lastMouseY
                );
            } else {
                follow.initialized = false;
            }

            const float radius = glm::length(follow.offset);

            transform.translation.x = targetTransform.translation.x +
                radius * std::cos(glm::radians(follow.pitch)) *
                std::sin(glm::radians(follow.yaw));

            transform.translation.y = targetTransform.translation.y +
                radius * std::sin(glm::radians(follow.pitch));

            transform.translation.z = targetTransform.translation.z +
                radius * std::cos(glm::radians(follow.pitch)) *
                std::cos(glm::radians(follow.yaw));

            transform.rotation = UTILS::lookRotation(
                targetTransform.translation - transform.translation
            );
        }
    }

} // namespace SGE::SYSTEMS
