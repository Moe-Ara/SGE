#ifndef SGE_SYSTEMS_CAMERASYSTEM_H
#define SGE_SYSTEMS_CAMERASYSTEM_H

#include "../ECS/ISystem.h"

namespace SGE::GRAPHICS {
    class Window;
}

namespace SGE::SYSTEMS {

    class CameraSystem : public ECS::ISystem {
    public:
        explicit CameraSystem(GRAPHICS::Window& window);

        void update(entt::registry& registry, float deltaTime) override;

    private:
        GRAPHICS::Window& window;
    };

} // namespace SGE::SYSTEMS

#endif // SGE_SYSTEMS_CAMERASYSTEM_H
