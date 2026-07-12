#ifndef SGE_ECS_ISYSTEM_H
#define SGE_ECS_ISYSTEM_H

#include <entt/entt.hpp>

namespace SGE::ECS {

    class ISystem {
    public:
        virtual ~ISystem() = default;
        virtual void update(entt::registry& registry, float deltaTime) = 0;
        virtual void onWorldReset() {}
    };

} // namespace SGE::ECS

#endif // SGE_ECS_ISYSTEM_H
