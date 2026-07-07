#ifndef SGE_SYSTEMS_RENDERSYSTEM_H
#define SGE_SYSTEMS_RENDERSYSTEM_H

#include "../ECS/ISystem.h"
#include "../Graphics/Shader.h"
#include "../Graphics/Environment.h"
#include <memory>

namespace SGE::SYSTEMS {

    class RenderSystem : public ECS::ISystem {
    public:
        RenderSystem(std::shared_ptr<GRAPHICS::Shader> shader, std::shared_ptr<GRAPHICS::Environment> environment);

        void update(entt::registry& registry, float deltaTime) override;

    private:
        std::shared_ptr<GRAPHICS::Shader> shader;
        std::shared_ptr<GRAPHICS::Environment> environment;
    };

} // namespace SGE::SYSTEMS

#endif // SGE_SYSTEMS_RENDERSYSTEM_H
