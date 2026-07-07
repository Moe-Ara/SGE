#ifndef SGE_SYSTEMS_PHYSICSSYSTEM_H
#define SGE_SYSTEMS_PHYSICSSYSTEM_H

#include "../ECS/ISystem.h"
#include <glm/glm.hpp>

namespace SGE::SYSTEMS {

    class PhysicsSystem : public ECS::ISystem {
    public:
        void update(entt::registry& registry, float deltaTime) override;

    private:
        glm::vec3 gravity{0.0f, -9.81f, 0.0f};
    };

} // namespace SGE::SYSTEMS

#endif // SGE_SYSTEMS_PHYSICSSYSTEM_H
