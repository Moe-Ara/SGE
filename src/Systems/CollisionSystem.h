#ifndef SGE_SYSTEMS_COLLISIONSYSTEM_H
#define SGE_SYSTEMS_COLLISIONSYSTEM_H

#include "../ECS/ISystem.h"
#include "../Physics/BVH.h"
#include "../Events/IEventSystem.h"
#include <memory>

namespace SGE::SYSTEMS {

    class CollisionSystem : public ECS::ISystem {
    public:
        explicit CollisionSystem(std::shared_ptr<EVENTS::IEventSystem> eventSystem);

        void update(entt::registry& registry, float deltaTime) override;

    private:
        std::shared_ptr<EVENTS::IEventSystem> eventSystem;
        PHYSICS::BVH bvh;

        static void resolveCollision(entt::registry& registry, const PHYSICS::CollisionInfo& collision);
    };

} // namespace SGE::SYSTEMS

#endif // SGE_SYSTEMS_COLLISIONSYSTEM_H
