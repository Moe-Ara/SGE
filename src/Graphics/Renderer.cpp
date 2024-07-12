//
// Created by Mohamad on 12/07/2024.
//

#include "Renderer.h"
#include "../Utils/Light.h"
#include "../Utils/Material.h"

SGE::GRAPHICS::Renderer::Renderer(std::shared_ptr<SGE::GRAPHICS::Shader> shader,
                                  std::shared_ptr<SGE::GRAPHICS::Camera> camera) :shader(shader),camera(camera){

}

void SGE::GRAPHICS::Renderer::render(const SGE::GRAPHICS::Scene &scene) {
    // Set light properties
    SGE::UTILS::Light light;
    light.position = glm::vec3(10.0f, 5.0f, 0.0f);
    light.ambient = glm::vec3(0.3f,0.3f, 0.3f);
    light.diffuse = glm::vec3(0.5f, 0.5f, 0.5f);
    light.specular = glm::vec3(1.0f, 1.0f, 1.0f);

    // Set material properties
    SGE::UTILS::Material material;
    material.ambient = glm::vec3(1.0f, 0.5f, 0.31f);
    material.diffuse = glm::vec3(1.0f, 0.5f, 0.31f);
    material.specular = glm::vec3(0.5f, 0.5f, 0.5f);
    material.shininess = 32.0f;
    shader->enable();



    shader->setUniformMat4("projection", camera->getProjection());
    shader->setUniformMat4("view", camera->getView());


    shader->setUniformFloat3("material.ambient", material.ambient);
    shader->setUniformFloat3("material.diffuse", material.diffuse);
    shader->setUniformFloat3("material.specular", material.specular);
    shader->setUniformFloat1("material.shininess", material.shininess);

    shader->setUniformFloat3("light.position", light.position);
    shader->setUniformFloat3("light.ambient", light.ambient);
    shader->setUniformFloat3("light.diffuse", light.diffuse);
    shader->setUniformFloat3("light.specular", light.specular);

    // Set view position (camera position)
    shader->setUniformFloat3("viewPos", camera->getPositionFromViewMatrix());


    for (const auto& object : scene.getGameObjects()) {
        renderObject(object);
    }

    shader->disable();
}

void SGE::GRAPHICS::Renderer::renderObject(const std::shared_ptr<SGE::GAMEOBJECTS::GameObject> &object) {
    shader->setUniformMat4("transform", object->getTransform().mat4());
    shader->setUniformMat3("normalMatrix", object->getTransform().normalMatrix());
    shader->setUniformFloat3("uniformColor", object->getColor());
    object->render();
}
