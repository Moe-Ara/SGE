#include "CollisionSystem.h"
#include "../ECS/Components.h"
#include "../Events/CollisionEvent.h"
#include <glm/geometric.hpp>

namespace SGE::SYSTEMS {

    CollisionSystem::CollisionSystem(std::shared_ptr<EVENTS::IEventSystem> eventSys)
        : eventSystem(std::move(eventSys)) {}

    void CollisionSystem::update(entt::registry& registry, float deltaTime) {
        (void)deltaTime;

        std::vector<PHYSICS::SpatialItem> items;
        auto view = registry.view<ECS::TransformComponent, ECS::SphereColliderComponent>();
        items.reserve(view.size_hint());
        for (auto entity : view) {
            const auto& transform = view.get<ECS::TransformComponent>(entity);
            const auto& collider = view.get<ECS::SphereColliderComponent>(entity);
            items.push_back(PHYSICS::SpatialItem{entity, transform.translation, collider.radius});
        }

        bvh.build(items);

        std::vector<PHYSICS::CollisionInfo> collisions;
        bvh.getCollisions(collisions);

        for (const auto& collision : collisions) {
            resolveCollision(registry, collision);

            if (eventSystem) {
                EVENTS::CollisionEvent event("CollisionEvent", collision.entityA, collision.entityB,
                                              collision.contactPoint, collision.normal);
                eventSystem->publish(event);
            }
        }
    }

    void CollisionSystem::resolveCollision(entt::registry& registry, const PHYSICS::CollisionInfo& collision) {
        if (!registry.valid(collision.entityA) || !registry.valid(collision.entityB)) {
            return;
        }

        auto* transformA = registry.try_get<ECS::TransformComponent>(collision.entityA);
        auto* transformB = registry.try_get<ECS::TransformComponent>(collision.entityB);
        if (!transformA || !transformB) {
            return;
        }

        auto* bodyA = registry.try_get<ECS::RigidBodyComponent>(collision.entityA);
        auto* bodyB = registry.try_get<ECS::RigidBodyComponent>(collision.entityB);

        const bool movableA = bodyA && !bodyA->isStatic;
        const bool movableB = bodyB && !bodyB->isStatic;
        if (!movableA && !movableB) {
            return;
        }

        // Positional correction: separate the two along the contact normal
        // (which points from A to B) proportional to how deep they overlap.
        const glm::vec3 correction = collision.normal * collision.penetrationDepth;
        if (!movableA) {
            transformB->translateBy(correction);
        } else if (!movableB) {
            transformA->translateBy(-correction);
        } else {
            transformA->translateBy(-correction * 0.5f);
            transformB->translateBy(correction * 0.5f);
        }

        // Velocity response: kill/reflect the closing component of each movable
        // body's velocity along the shared normal, scaled by its own restitution.
        if (movableA) {
            const float speedAlongNormal = glm::dot(bodyA->velocity, collision.normal);
            if (speedAlongNormal > 0.0f) {
                bodyA->velocity -= (1.0f + bodyA->restitution) * speedAlongNormal * collision.normal;
            }
        }
        if (movableB) {
            const float speedAlongNormal = glm::dot(bodyB->velocity, collision.normal);
            if (speedAlongNormal < 0.0f) {
                bodyB->velocity -= (1.0f + bodyB->restitution) * speedAlongNormal * collision.normal;
            }
        }
    }

} // namespace SGE::SYSTEMS
