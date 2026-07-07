#include "PhysicsSystem.h"
#include "../ECS/Components.h"

namespace SGE::SYSTEMS {

    void PhysicsSystem::update(entt::registry& registry, float deltaTime) {
        auto view = registry.view<ECS::TransformComponent, ECS::RigidBodyComponent>();
        for (auto entity : view) {
            auto& transform = view.get<ECS::TransformComponent>(entity);
            auto& body = view.get<ECS::RigidBodyComponent>(entity);

            if (body.isStatic || !body.enabled) {
                continue;
            }

            body.addForce(gravity * body.mass);

            body.acceleration = body.force / body.mass;
            body.velocity += body.acceleration * deltaTime;
            transform.translation += body.velocity * deltaTime;

            body.clearForces();
        }
    }

} // namespace SGE::SYSTEMS
