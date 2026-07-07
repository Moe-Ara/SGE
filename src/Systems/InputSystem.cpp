#include "InputSystem.h"
#include "../ECS/Components.h"
#include "../ECS/Queries.h"
#include <GLFW/glfw3.h>
#include <glm/geometric.hpp>

namespace SGE::SYSTEMS {

    InputSystem::InputSystem(std::shared_ptr<INPUT::InputHandler> handler)
        : inputHandler(std::move(handler)) {}

    void InputSystem::update(entt::registry& registry, float deltaTime) {
        if (!inputHandler) {
            return;
        }

        glm::vec3 forward{0.0f, 0.0f, -1.0f};
        glm::vec3 right{1.0f, 0.0f, 0.0f};

        if (const entt::entity primaryCamera = ECS::findPrimaryCamera(registry); primaryCamera != entt::null) {
            const auto& cam = registry.get<ECS::CameraComponent>(primaryCamera);
            forward = cam.camera.getForward();
            forward.y = 0.0f;
            if (glm::length(forward) > 0.0001f) {
                forward = glm::normalize(forward);
            }
            right = cam.camera.getRight();
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

            const bool wantsUp = inputHandler->isKeyPressed(GLFW_KEY_SPACE) ||
                                  inputHandler->isKeyPressed(GLFW_KEY_LEFT_SHIFT);

            // If this entity also has a RigidBodyComponent, PhysicsSystem owns
            // its TransformComponent::translation (it's the only thing that
            // integrates velocity into position). Express input as a desired
            // velocity instead of writing translation directly, so the two
            // systems don't fight over the same field.
            if (auto* body = registry.try_get<ECS::RigidBodyComponent>(entity)) {
                body->velocity.x = movement.x;
                body->velocity.z = movement.z;
                if (wantsUp) {
                    body->velocity.y = controller.jumpSpeed;
                }
                // else: leave velocity.y untouched so gravity keeps integrating it.
            } else {
                transform.translateBy(movement * deltaTime);
                if (wantsUp) {
                    transform.translateBy(glm::vec3(0.0f, controller.jumpSpeed, 0.0f) * deltaTime);
                }
            }
        }
    }

} // namespace SGE::SYSTEMS
