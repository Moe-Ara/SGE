#ifndef SGE_SYSTEMS_THIRDPERSONCAMERACONTROLLERSYSTEM_H
#define SGE_SYSTEMS_THIRDPERSONCAMERACONTROLLERSYSTEM_H

#include "../ECS/ISystem.h"
#include "../Input/InputHandler.h"
#include <memory>

namespace SGE::GRAPHICS {
    class Window;
}

namespace SGE::SYSTEMS {

    class ThirdPersonCameraControllerSystem : public ECS::ISystem {
    public:
        ThirdPersonCameraControllerSystem(GRAPHICS::Window& window, std::shared_ptr<INPUT::InputHandler> inputHandler);
        void update(entt::registry& registry, float deltaTime) override;

    private:
        GRAPHICS::Window& window;
        std::shared_ptr<INPUT::InputHandler> inputHandler;
    };

} // namespace SGE::SYSTEMS

#endif // SGE_SYSTEMS_THIRDPERSONCAMERACONTROLLERSYSTEM_H
