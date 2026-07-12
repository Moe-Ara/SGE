#include "PlayerControllerSystem.h"
#include "../ECS/Components.h"
#include "../ECS/Queries.h"
#include "../Utils/CameraMath.h"

#include <GLFW/glfw3.h>
#include <glm/geometric.hpp>

namespace SGE::SYSTEMS {

    PlayerControllerSystem::PlayerControllerSystem(std::shared_ptr<INPUT::InputHandler> inputHandler)
        : inputHandler(std::move(inputHandler)) {}

    void PlayerControllerSystem::update(entt::registry& registry, float deltaTime) {
        if (!inputHandler) {
            return;
        }

        glm::vec3 forward{0.0f, 0.0f, -1.0f};
        glm::vec3 right{1.0f, 0.0f, 0.0f};

        if (const entt::entity primaryCamera = ECS::findPrimaryCamera(registry); primaryCamera != entt::null) {
            const auto* cameraTransform = registry.try_get<ECS::TransformComponent>(primaryCamera);
            if (cameraTransform) {
                forward = UTILS::cameraForward(cameraTransform->rotation);
                right = UTILS::rightFromForward(forward);
            }
            forward.y = 0.0f;
            if (glm::length(forward) > 0.0001f) {
                forward = glm::normalize(forward);
            }
            right.y = 0.0f;
            if (glm::length(right) > 0.0001f) {
                right = glm::normalize(right);
            }
        }

        auto view = registry.view<ECS::TransformComponent, ECS::PlayerControllerComponent>();
        for (auto entity : view) {
            auto& transform = view.get<ECS::TransformComponent>(entity);
            auto& controller = view.get<ECS::PlayerControllerComponent>(entity);

            glm::vec3 movement{0.0f};
            if (inputHandler->isKeyPressed(GLFW_KEY_W)) movement += forward;
            if (inputHandler->isKeyPressed(GLFW_KEY_S)) movement -= forward;
            if (inputHandler->isKeyPressed(GLFW_KEY_A)) movement -= right;
            if (inputHandler->isKeyPressed(GLFW_KEY_D)) movement += right;
            if (glm::length(movement) > 0.0001f) {
                movement = glm::normalize(movement) * controller.movementSpeed;
            }

            const bool wantsJump = inputHandler->isKeyJustPressed(GLFW_KEY_SPACE) &&
                                   controller.grounded;

            if (auto* body = registry.try_get<ECS::RigidBodyComponent>(entity)) {
                body->velocity.x = movement.x;
                body->velocity.z = movement.z;
                if (wantsJump) {
                    body->velocity.y = controller.jumpSpeed;
                    controller.grounded = false;
                }
            } else {
                transform.translateBy(movement * deltaTime);
                if (wantsJump) {
                    transform.translateBy(glm::vec3(0.0f, controller.jumpSpeed, 0.0f) * deltaTime);
                    controller.grounded = false;
                }
            }
        }
    }

} // namespace SGE::SYSTEMS
