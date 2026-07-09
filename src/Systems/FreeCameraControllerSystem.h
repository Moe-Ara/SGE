#ifndef SGE_SYSTEMS_FREECAMERACONTROLLERSYSTEM_H
#define SGE_SYSTEMS_FREECAMERACONTROLLERSYSTEM_H

#include "../ECS/ISystem.h"
#include "../Input/InputHandler.h"
#include <memory>

namespace SGE::GRAPHICS {
    class Window;
}

namespace SGE::SYSTEMS {

    class FreeCameraControllerSystem : public ECS::ISystem {
    public:
        FreeCameraControllerSystem(GRAPHICS::Window& window, std::shared_ptr<INPUT::InputHandler> inputHandler);
        void update(entt::registry& registry, float deltaTime) override;

    private:
        GRAPHICS::Window& window;
        std::shared_ptr<INPUT::InputHandler> inputHandler;
    };

} // namespace SGE::SYSTEMS

#endif // SGE_SYSTEMS_FREECAMERACONTROLLERSYSTEM_H
