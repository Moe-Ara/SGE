#ifndef SGE_SYSTEMS_INPUTSYSTEM_H
#define SGE_SYSTEMS_INPUTSYSTEM_H

#include "../ECS/ISystem.h"
#include "../Input/InputHandler.h"
#include <memory>

namespace SGE::SYSTEMS {

    class InputSystem : public ECS::ISystem {
    public:
        explicit InputSystem(std::shared_ptr<INPUT::InputHandler> inputHandler);

        void update(entt::registry& registry, float deltaTime) override;

    private:
        std::shared_ptr<INPUT::InputHandler> inputHandler;
    };

} // namespace SGE::SYSTEMS

#endif // SGE_SYSTEMS_INPUTSYSTEM_H
