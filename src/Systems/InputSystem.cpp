#include "InputSystem.h"

namespace SGE::SYSTEMS {

    InputSystem::InputSystem(std::shared_ptr<INPUT::InputHandler> handler)
        : inputHandler(std::move(handler)) {}

    void InputSystem::update(entt::registry& registry, float deltaTime) {
        (void)registry;
        (void)deltaTime;
    }

} // namespace SGE::SYSTEMS
