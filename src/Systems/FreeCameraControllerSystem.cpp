#include "FreeCameraControllerSystem.h"
#include "../ECS/Components.h"
#include "../Graphics/Window.h"
#include "../Utils/CameraMath.h"

#include <GLFW/glfw3.h>
#include <glm/common.hpp>
#include <glm/geometric.hpp>

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

            const bool rotating = inputHandler->isMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT);

            if (!rotating) {
                glfwSetInputMode(window.getMWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                freeCam.initialized = false;
                continue;
            }

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
            freeCam.pitch += static_cast<float>(deltaY) * freeCam.mouseSensitivity;
            freeCam.pitch = glm::clamp(freeCam.pitch, -89.0f, 89.0f);

            freeCam.lastMouseX = mouseX;
            freeCam.lastMouseY = mouseY;

            const glm::vec3 forward = UTILS::forwardFromYawPitch(freeCam.yaw, freeCam.pitch);
            const glm::vec3 right =
                glm::normalize(glm::cross(forward, UTILS::worldUp()));
            const glm::vec3 up = UTILS::worldUp();

            float speed = freeCam.movementSpeed;
            if (inputHandler->isKeyPressed(GLFW_KEY_LEFT_SHIFT)) {
                speed *= freeCam.sprintMultiplier;
            }

            if (inputHandler->isKeyPressed(GLFW_KEY_W)) {
                transform.translation += forward * speed * deltaTime;
            }
            if (inputHandler->isKeyPressed(GLFW_KEY_S)) {
                transform.translation -= forward * speed * deltaTime;
            }
            if (inputHandler->isKeyPressed(GLFW_KEY_D)) {
                transform.translation += right * speed * deltaTime;
            }
            if (inputHandler->isKeyPressed(GLFW_KEY_A)) {
                transform.translation -= right * speed * deltaTime;
            }
            if (inputHandler->isKeyPressed(GLFW_KEY_E)) {
                transform.translation += up * speed * deltaTime;
            }
            if (inputHandler->isKeyPressed(GLFW_KEY_Q)) {
                transform.translation -= up * speed * deltaTime;
            }

            transform.rotation = UTILS::lookRotation(forward);
        }
    }

} // namespace SGE::SYSTEMS
