#include "CollisionSystem.h"
#include "../ECS/Components.h"
#include "../Events/CollisionEvent.h"
#include <glm/common.hpp>
#include <glm/geometric.hpp>
#include <algorithm>

namespace SGE::SYSTEMS {

    std::size_t CollisionSystem::EntityPairHash::operator()(const EntityPair& pair) const noexcept {
        const std::size_t first = static_cast<std::size_t>(entt::to_integral(pair.first));
        const std::size_t second = static_cast<std::size_t>(entt::to_integral(pair.second));
        return first ^ (second + 0x9e3779b9u + (first << 6u) + (first >> 2u));
    }

    CollisionSystem::EntityPair CollisionSystem::makePair(entt::entity a, entt::entity b) {
        return entt::to_integral(a) < entt::to_integral(b)
            ? EntityPair{a, b}
            : EntityPair{b, a};
    }

    PHYSICS::CollisionInfo CollisionSystem::canonicalize(
        const PHYSICS::CollisionInfo& collision,
        const EntityPair& pair
    ) {
        if (collision.entityA == pair.first) {
            return collision;
        }
        PHYSICS::CollisionInfo canonical = collision;
        canonical.entityA = pair.first;
        canonical.entityB = pair.second;
        canonical.normal = -canonical.normal;
        return canonical;
    }

    CollisionSystem::CollisionSystem(std::shared_ptr<EVENTS::IEventSystem> eventSys)
        : eventSystem(std::move(eventSys)) {}

    void CollisionSystem::onWorldReset() {
        spatialItems.clear();
        collisions.clear();
        previousContacts.clear();
        currentContacts.clear();
    }

    void CollisionSystem::update(entt::registry& registry, float deltaTime) {
        (void)deltaTime;

        auto controllerView = registry.view<ECS::PlayerControllerComponent>();
        for (auto entity : controllerView) {
            controllerView.get<ECS::PlayerControllerComponent>(entity).grounded = false;
        }

        spatialItems.clear();
        auto view = registry.view<ECS::TransformComponent, ECS::SphereColliderComponent>();
        spatialItems.reserve(view.size_hint());
        for (auto entity : view) {
            const auto& transform = view.get<ECS::TransformComponent>(entity);
            const auto& collider = view.get<ECS::SphereColliderComponent>(entity);
            if (const auto* body = registry.try_get<ECS::RigidBodyComponent>(entity);
                body && !body->enabled) {
                continue;
            }
            const glm::vec3 absoluteScale = glm::abs(transform.scale);
            const float scale = std::max({absoluteScale.x, absoluteScale.y, absoluteScale.z});
            spatialItems.push_back(PHYSICS::SpatialItem{
                entity,
                transform.translation,
                std::max(collider.radius * scale, 0.0f)
            });
        }

        bvh.build(spatialItems);

        collisions.clear();
        bvh.getCollisions(collisions);
        currentContacts.clear();
        currentContacts.reserve(collisions.size());

        for (const auto& collision : collisions) {
            const EntityPair pair = makePair(collision.entityA, collision.entityB);
            const PHYSICS::CollisionInfo canonicalCollision = canonicalize(collision, pair);
            const EVENTS::CollisionPhase phase = previousContacts.contains(pair)
                ? EVENTS::CollisionPhase::Stay
                : EVENTS::CollisionPhase::Enter;
            currentContacts.insert_or_assign(pair, canonicalCollision);

            if (auto* controller = registry.try_get<ECS::PlayerControllerComponent>(collision.entityA)) {
                controller->grounded = controller->grounded || (-collision.normal.y) > 0.5f;
            }
            if (auto* controller = registry.try_get<ECS::PlayerControllerComponent>(collision.entityB)) {
                controller->grounded = controller->grounded || collision.normal.y > 0.5f;
            }
            resolveCollision(registry, collision);

            publishContact(canonicalCollision, phase);
        }

        for (const auto& [pair, previousCollision] : previousContacts) {
            if (!currentContacts.contains(pair)) {
                publishContact(previousCollision, EVENTS::CollisionPhase::Exit);
            }
        }

        previousContacts.swap(currentContacts);
    }

    void CollisionSystem::publishContact(const PHYSICS::CollisionInfo& collision,
                                         EVENTS::CollisionPhase phase) const {
        if (!eventSystem) {
            return;
        }
        const char* name = phase == EVENTS::CollisionPhase::Enter
            ? "CollisionEnter"
            : (phase == EVENTS::CollisionPhase::Stay ? "CollisionStay" : "CollisionExit");
        EVENTS::CollisionEvent event(name, collision.entityA, collision.entityB,
                                     collision.contactPoint, collision.normal, phase);
        eventSystem->publish(event);
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

        const bool movableA = bodyA && bodyA->enabled && !bodyA->isStatic && bodyA->mass > 0.0f;
        const bool movableB = bodyB && bodyB->enabled && !bodyB->isStatic && bodyB->mass > 0.0f;
        if (!movableA && !movableB) {
            return;
        }

        const float inverseMassA = movableA ? 1.0f / bodyA->mass : 0.0f;
        const float inverseMassB = movableB ? 1.0f / bodyB->mass : 0.0f;
        const float inverseMassSum = inverseMassA + inverseMassB;

        const glm::vec3 correction =
            collision.normal * (collision.penetrationDepth / inverseMassSum);
        transformA->translateBy(-correction * inverseMassA);
        transformB->translateBy(correction * inverseMassB);

        const glm::vec3 velocityA = movableA ? bodyA->velocity : glm::vec3{0.0f};
        const glm::vec3 velocityB = movableB ? bodyB->velocity : glm::vec3{0.0f};
        const float closingSpeed = glm::dot(velocityB - velocityA, collision.normal);
        if (closingSpeed < 0.0f) {
            const float restitutionA = movableA ? glm::clamp(bodyA->restitution, 0.0f, 1.0f) : 1.0f;
            const float restitutionB = movableB ? glm::clamp(bodyB->restitution, 0.0f, 1.0f) : 1.0f;
            const float restitution = std::min(restitutionA, restitutionB);
            const glm::vec3 impulse =
                (-(1.0f + restitution) * closingSpeed / inverseMassSum) * collision.normal;
            if (movableA) bodyA->velocity -= impulse * inverseMassA;
            if (movableB) bodyB->velocity += impulse * inverseMassB;
        }
    }

} // namespace SGE::SYSTEMS
