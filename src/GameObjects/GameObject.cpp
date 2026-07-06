//
// Created by Mohamad on 11/07/2024.
//

#include "GameObject.h"

#include <utility>

namespace SGE::GAMEOBJECTS {

GameObject::GameObject(Transform transform, std::shared_ptr<Model> model)
    : id(""), name(""), transform(transform), model(std::move(model)), physicsComponent(nullptr), eventSystem(nullptr) {
}

std::string GameObject::getId() const {
    return id;
}

std::string GameObject::getName() const {
    return name;
}

Transform GameObject::getTransform() const {
    return transform;
}

void GameObject::setTransform(const Transform& t) {
    transform = t;
}

Transform& GameObject::getTransformRef() {
    return transform;
}

std::shared_ptr<PHYSICS::IPhysicsComponent> GameObject::getPhysicsComponent() const {
    return physicsComponent;
}

void GameObject::setPhysicsComponent(std::shared_ptr<PHYSICS::IPhysicsComponent> component) {
    physicsComponent = std::move(component);
}

void GameObject::setEventSystem(std::shared_ptr<EVENTS::IEventSystem> eventSys) {
    eventSystem = std::move(eventSys);
}

std::shared_ptr<EVENTS::IEventSystem> GameObject::getEventSystem() const {
    return eventSystem;
}

void GameObject::update(float deltaTime) {
    (void)deltaTime;
}

glm::vec3 GameObject::getColor() const {
    return glm::vec3(1.0f, 1.0f, 1.0f);
}

void GameObject::render() {
    if (model) {
        model->bind();
        model->render();
        model->unbind();
    }
}

} // namespace SGE::GAMEOBJECTS
