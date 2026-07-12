#include "RenderSystem.h"
#include "../ECS/Components.h"
#include "../ECS/Queries.h"
#include <string>

namespace SGE::SYSTEMS {

    namespace {
        constexpr int kMaxLights = 4;
    }

    RenderSystem::RenderSystem(std::shared_ptr<GRAPHICS::Shader> shaderProgram,
                                std::shared_ptr<GRAPHICS::Environment> env)
        : shader(std::move(shaderProgram)), environment(std::move(env)) {}

    void RenderSystem::update(entt::registry& registry, float deltaTime) {
        (void)deltaTime;
        if (!shader) {
            return;
        }

        const entt::entity primaryCamera = ECS::findPrimaryCamera(registry);
        if (primaryCamera == entt::null) {
            return;
        }
        const auto* activeCamera = &registry.get<ECS::CameraComponent>(primaryCamera);

        shader->use();
        shader->setVec3("viewPos", activeCamera->camera.getPosition());
        shader->setMat4("projection", activeCamera->camera.getProjectionMatrix());
        shader->setMat4("view", activeCamera->camera.getViewMatrix());

        if (environment) {
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_CUBE_MAP, environment->getIrradianceMapId());
            shader->setInt("irradianceMap", 1);

            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_CUBE_MAP, environment->getPrefilterMapId());
            shader->setInt("prefilterMap", 2);

            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_2D, environment->getBRDFLUTId());
            shader->setInt("brdfLUT", 3);

            shader->setFloat("prefilterMaxLod", environment->getPrefilterMaxLod());
        }

        int lightCount = 0;
        auto lightView = registry.view<ECS::LightComponent>();
        for (auto entity : lightView) {
            if (lightCount >= kMaxLights) {
                break;
            }
            const auto& lightComp = lightView.get<ECS::LightComponent>(entity);
            const glm::vec3 position = registry.all_of<ECS::TransformComponent>(entity)
                ? registry.get<ECS::TransformComponent>(entity).translation
                : lightComp.light.position;
            shader->setPBRLight("lights[" + std::to_string(lightCount) + "]",
                                 position, lightComp.light.color, lightComp.light.intensity);
            ++lightCount;
        }
        shader->setInt("numLights", lightCount);

        auto view = registry.view<ECS::TransformComponent, ECS::MeshComponent>();
        for (auto entity : view) {
            const auto& transform = view.get<ECS::TransformComponent>(entity);
            const auto& mesh = view.get<ECS::MeshComponent>(entity);
            if (!mesh.model) {
                continue;
            }

            shader->setMat4("transform", transform.mat4());
            shader->setMat3("normalMatrix", transform.normalMatrix());

            if (const auto* material = registry.try_get<ECS::MaterialComponent>(entity)) {
                shader->setPBRMaterial(material->albedo, material->metallic, material->roughness, material->ao);
                if (material->albedoTexture) {
                    material->albedoTexture->bind(0);
                    shader->setInt("albedoMap", 0);
                    shader->setInt("useAlbedoMap", 1);
                } else {
                    shader->setInt("useAlbedoMap", 0);
                }
            } else {
                shader->setPBRMaterial(glm::vec3(1.0f), 0.0f, 0.5f, 1.0f);
                shader->setInt("useAlbedoMap", 0);
            }

            mesh.model->render();
        }
    }

} // namespace SGE::SYSTEMS
