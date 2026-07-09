#ifndef SGE_SYSTEMS_CAMERARENDERSYSTEM_H
#define SGE_SYSTEMS_CAMERARENDERSYSTEM_H

#include "../ECS/ISystem.h"

namespace SGE::GRAPHICS {
    class Window;
}

namespace SGE::SYSTEMS {

    class CameraRenderSystem : public ECS::ISystem {
    public:
        explicit CameraRenderSystem(GRAPHICS::Window& window);
        void update(entt::registry& registry, float deltaTime) override;

    private:
        void updateProjection(entt::registry& registry) const;
        void updateView(entt::registry& registry) const;

        GRAPHICS::Window& window;
    };

} // namespace SGE::SYSTEMS

#endif // SGE_SYSTEMS_CAMERARENDERSYSTEM_H
