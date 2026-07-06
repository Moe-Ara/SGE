#include "Renderer.h"
#include "Camera.h"
#include "../GameObjects/Actor.h"
#include <utility>

namespace SGE::GRAPHICS {

Renderer::Renderer() : shader(nullptr), camera(nullptr), shadowsEnabled(false) {
    initialize();
}

Renderer::Renderer(std::shared_ptr<Shader> shader, std::shared_ptr<Camera> camera)
    : shader(std::move(shader)), camera(std::move(camera)), shadowsEnabled(false) {
    initialize();
}

Renderer::~Renderer() = default;

void Renderer::initialize() {
    // Initialize renderer components
}

void Renderer::render(const std::shared_ptr<Camera>& camera,
                      const std::vector<std::shared_ptr<SGE::GAMEOBJECTS::Actor>>& actors) {
    if (!shader || !camera) {
        return;
    }

    this->camera = camera;

    const glm::mat4 view = camera->getViewMatrix();
    const glm::mat4 projection = camera->getProjectionMatrix();

    shader->use();
    shader->setVec3("viewPos", camera->getPosition());
    shader->setMat4("projection", projection);
    shader->setMat4("view", view);

    for (const auto& actor : actors) {
        if (actor) {
            actor->render(shader);
        }
    }
}

void Renderer::setupPBRRendering(const std::shared_ptr<Shader>& shader) {
    if (shader && camera) {
        shader->use();
        shader->setVec3("viewPos", camera->getPosition());
    }
}

void Renderer::setPBRMaterial(const std::shared_ptr<Shader>& shader,
                              const glm::vec3& albedo, float metallic, float roughness, float ao) {
    if (shader) {
        shader->setPBRMaterial(albedo, metallic, roughness, ao);
    }
}

void Renderer::setPBRLight(const std::shared_ptr<Shader>& shader,
                           const std::string& lightName,
                           const glm::vec3& position,
                           const glm::vec3& color,
                           float intensity) {
    if (shader) {
        shader->setPBRLight(lightName, position, color, intensity);
    }
}

void Renderer::setupShadowRendering(const std::shared_ptr<Shader>& shader,
                                    const glm::mat4& lightSpaceMatrix,
                                    const glm::vec3& lightPos) {
    if (shader) {
        shader->use();
        shader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
        shader->setVec3("lightPos", lightPos);
    }
}

} // namespace SGE::GRAPHICS
