#ifndef SGE_SYSTEMS_SKYBOXSYSTEM_H
#define SGE_SYSTEMS_SKYBOXSYSTEM_H

#include "../ECS/ISystem.h"
#include "../Graphics/Environment.h"
#include "../Graphics/Shader.h"
#include <memory>

namespace SGE::SYSTEMS {

    // Draws the baked environment cubemap as the scene background, behind
    // everything RenderSystem already drew this frame.
    class SkyboxSystem : public ECS::ISystem {
    public:
        SkyboxSystem(std::shared_ptr<GRAPHICS::Environment> environment,
                     std::shared_ptr<GRAPHICS::Shader> skyboxShader);

        void update(entt::registry& registry, float deltaTime) override;

    private:
        std::shared_ptr<GRAPHICS::Environment> environment;
        std::shared_ptr<GRAPHICS::Shader> shader;
    };

} // namespace SGE::SYSTEMS

#endif // SGE_SYSTEMS_SKYBOXSYSTEM_H
