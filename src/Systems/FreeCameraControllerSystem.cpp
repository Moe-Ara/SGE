#include "FreeCameraControllerSystem.h"
#include "../ECS/Components.h"
#include "../Graphics/Window.h"
#include "../Utils/CameraMath.h"

#include <GLFW/glfw3.h>
#include <glm/common.hpp>
#include <glm/geometric.hpp>
#include <algorithm>
#include <cmath>

namespace SGE::SYSTEMS {

    FreeCameraControllerSystem::FreeCameraControllerSystem(
        GRAPHICS::Window& window,
        std::shared_ptr<INPUT::InputHandler> inputHandler
    ) : window(window), inputHandler(std::move(inputHandler)) {}

    void FreeCameraControllerSystem::update(entt::registry& registry, float deltaTime) {
        if (window.isHeadless() || !inputHandler) {
            return;
        }

        auto view = registry.view<
            ECS::CameraComponent,
            ECS::TransformComponent,
            ECS::FreeCameraComponent
        >();

        for (auto entity : view) {
            auto& transform = view.get<ECS::TransformComponent>(entity);
            auto& freeCam = view.get<ECS::FreeCameraComponent>(entity);

            if (!freeCam.enabled) {
                continue;
            }

            const bool rightMouseDown =
                inputHandler->isMouseButtonPressedRaw(GLFW_MOUSE_BUTTON_RIGHT);
            if (!rightMouseDown) {
                freeCam.mouseLookActive = false;
            } else if (!freeCam.mouseLookActive && !inputHandler->isMouseCaptured()) {
                freeCam.mouseLookActive = true;
            }
            const bool rotating = freeCam.mouseLookActive;

            if (rotating) {
                glfwSetInputMode(window.getMWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

                const double mouseX = INPUT::InputHandler::getMouseX();
                const double mouseY = INPUT::InputHandler::getMouseY();

                if (!freeCam.initialized) {
                    freeCam.lastMouseX = mouseX;
                    freeCam.lastMouseY = mouseY;
                    freeCam.initialized = true;
                }

                const double deltaX = mouseX - freeCam.lastMouseX;
                const double deltaY = freeCam.lastMouseY - mouseY;

                freeCam.yaw += static_cast<float>(deltaX) * freeCam.mouseSensitivity;
                freeCam.pitch = UTILS::clampPitch(
                    freeCam.pitch + static_cast<float>(deltaY) * freeCam.mouseSensitivity
                );

                freeCam.lastMouseX = mouseX;
                freeCam.lastMouseY = mouseY;
            } else {
                glfwSetInputMode(window.getMWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                freeCam.initialized = false;
            }

            if (!freeCam.rotationInitialized) {
                freeCam.smoothedYaw = freeCam.yaw;
                freeCam.smoothedPitch = freeCam.pitch;
                freeCam.rotationInitialized = true;
            }

            const float safeDeltaTime = std::max(deltaTime, 0.0f);
            const float responsiveness = std::max(freeCam.rotationResponsiveness, 0.0f);
            const float rotationBlend = 1.0f - std::exp(-responsiveness * safeDeltaTime);
            freeCam.smoothedYaw = glm::mix(freeCam.smoothedYaw, freeCam.yaw, rotationBlend);
            freeCam.smoothedPitch = glm::mix(freeCam.smoothedPitch, freeCam.pitch, rotationBlend);

            const glm::vec3 forward = UTILS::forwardFromYawPitch(
                freeCam.smoothedYaw,
                freeCam.smoothedPitch
            );
            const glm::vec3 right = UTILS::rightFromForward(forward);
            const glm::vec3 up = UTILS::worldUp();
            const auto movementKeyPressed = [&](int key) {
                return rotating
                    ? inputHandler->isKeyPressedRaw(key)
                    : inputHandler->isKeyPressed(key);
            };

            float speed = freeCam.movementSpeed;
            if (movementKeyPressed(GLFW_KEY_LEFT_SHIFT)) {
                speed *= freeCam.sprintMultiplier;
            }

            if (movementKeyPressed(GLFW_KEY_W)) {
                transform.translation += forward * speed * deltaTime;
            }
            if (movementKeyPressed(GLFW_KEY_S)) {
                transform.translation -= forward * speed * deltaTime;
            }
            if (movementKeyPressed(GLFW_KEY_D)) {
                transform.translation += right * speed * deltaTime;
            }
            if (movementKeyPressed(GLFW_KEY_A)) {
                transform.translation -= right * speed * deltaTime;
            }
            if (movementKeyPressed(GLFW_KEY_E)) {
                transform.translation += up * speed * deltaTime;
            }
            if (movementKeyPressed(GLFW_KEY_Q)) {
                transform.translation -= up * speed * deltaTime;
            }

            transform.rotation = UTILS::rotationFromYawPitch(
                freeCam.smoothedYaw,
                freeCam.smoothedPitch
            );
        }
    }

} // namespace SGE::SYSTEMS
