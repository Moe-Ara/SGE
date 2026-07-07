#include "SkyboxSystem.h"
#include "../ECS/Components.h"
#include "../ECS/Queries.h"

namespace SGE::SYSTEMS {

    SkyboxSystem::SkyboxSystem(std::shared_ptr<GRAPHICS::Environment> env,
                                std::shared_ptr<GRAPHICS::Shader> skyboxShader)
        : environment(std::move(env)), shader(std::move(skyboxShader)) {}

    void SkyboxSystem::update(entt::registry& registry, float deltaTime) {
        (void)deltaTime;
        if (!environment || !shader) {
            return;
        }

        const entt::entity primaryCamera = ECS::findPrimaryCamera(registry);
        if (primaryCamera == entt::null) {
            return;
        }
        const auto& cam = registry.get<ECS::CameraComponent>(primaryCamera);

        glDepthFunc(GL_LEQUAL);
        glDisable(GL_CULL_FACE);

        shader->use();
        shader->setMat4("projection", cam.camera.getProjectionMatrix());
        shader->setMat4("view", cam.camera.getViewMatrix());
        shader->setInt("environmentMap", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, environment->getEnvironmentCubemapId());

        glBindVertexArray(environment->getCubeVAO());
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

        glEnable(GL_CULL_FACE);
        glDepthFunc(GL_LESS);
    }

} // namespace SGE::SYSTEMS
