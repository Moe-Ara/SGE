#ifndef SGE_SYSTEMS_COLLISIONSYSTEM_H
#define SGE_SYSTEMS_COLLISIONSYSTEM_H

#include "../ECS/ISystem.h"
#include "../Physics/BVH.h"
#include "../Events/IEventSystem.h"
#include "../Events/CollisionEvent.h"
#include <memory>
#include <vector>
#include <unordered_map>

namespace SGE::SYSTEMS {

    class CollisionSystem : public ECS::ISystem {
    public:
        explicit CollisionSystem(std::shared_ptr<EVENTS::IEventSystem> eventSystem);

        void update(entt::registry& registry, float deltaTime) override;
        void onWorldReset() override;

    private:
        struct EntityPair {
            entt::entity first;
            entt::entity second;

            bool operator==(const EntityPair&) const = default;
        };

        struct EntityPairHash {
            std::size_t operator()(const EntityPair& pair) const noexcept;
        };

        std::shared_ptr<EVENTS::IEventSystem> eventSystem;
        PHYSICS::BVH bvh;
        std::vector<PHYSICS::SpatialItem> spatialItems;
        std::vector<PHYSICS::CollisionInfo> collisions;
        std::unordered_map<EntityPair, PHYSICS::CollisionInfo, EntityPairHash> previousContacts;
        std::unordered_map<EntityPair, PHYSICS::CollisionInfo, EntityPairHash> currentContacts;

        static void resolveCollision(entt::registry& registry, const PHYSICS::CollisionInfo& collision);
        static EntityPair makePair(entt::entity a, entt::entity b);
        static PHYSICS::CollisionInfo canonicalize(const PHYSICS::CollisionInfo& collision,
                                                    const EntityPair& pair);
        void publishContact(const PHYSICS::CollisionInfo& collision,
                            EVENTS::CollisionPhase phase) const;
    };

} // namespace SGE::SYSTEMS

#endif // SGE_SYSTEMS_COLLISIONSYSTEM_H
