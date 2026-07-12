#include "PhysicsSystem.h"
#include "../ECS/Components.h"
#include <cmath>
#include <stdexcept>

namespace SGE::SYSTEMS {

    void PhysicsSystem::update(entt::registry& registry, float deltaTime) {
        auto view = registry.view<ECS::TransformComponent, ECS::RigidBodyComponent>();
        for (auto entity : view) {
            auto& transform = view.get<ECS::TransformComponent>(entity);
            auto& body = view.get<ECS::RigidBodyComponent>(entity);

            if (body.isStatic || !body.enabled) {
                continue;
            }

            if (!std::isfinite(body.mass) || body.mass <= 0.0f) {
                throw std::logic_error("Dynamic rigid body mass must be finite and positive");
            }

            body.addForce(gravity * body.mass);

            body.acceleration = body.force / body.mass;
            body.velocity += body.acceleration * deltaTime;
            transform.translation += body.velocity * deltaTime;

            body.clearForces();
        }
    }

} // namespace SGE::SYSTEMS
