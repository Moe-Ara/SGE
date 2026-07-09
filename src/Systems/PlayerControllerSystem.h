#ifndef SGE_SYSTEMS_PLAYERCONTROLLERSYSTEM_H
#define SGE_SYSTEMS_PLAYERCONTROLLERSYSTEM_H

#include "../ECS/ISystem.h"
#include "../Input/InputHandler.h"
#include <memory>

namespace SGE::SYSTEMS {

    class PlayerControllerSystem : public ECS::ISystem {
    public:
        explicit PlayerControllerSystem(std::shared_ptr<INPUT::InputHandler> inputHandler);
        void update(entt::registry& registry, float deltaTime) override;

    private:
        std::shared_ptr<INPUT::InputHandler> inputHandler;
    };

} // namespace SGE::SYSTEMS

#endif // SGE_SYSTEMS_PLAYERCONTROLLERSYSTEM_H
