//
// Created by Mohamad on 11/07/2024.
//
#include "Actor.h"

#include <utility>

namespace SGE::GAMEOBJECTS {

    Actor::Actor(long ID, Transform transform, std::shared_ptr<Model> model)
        : ID(ID), mass(1.0f), velocity(0.0f), accumulatedForces(0.0f),
          GameObject(transform, std::move(model)) {
    }

    void Actor::applyForce(const glm::vec3 &force) {
        accumulatedForces += force;
    }

    void Actor::update(float deltaTime) {
        glm::vec3 acceleration = accumulatedForces / mass;
        velocity += acceleration * deltaTime;
        move(velocity, deltaTime);
        accumulatedForces = glm::vec3(0.0f);
    }

    void Actor::move(glm::vec3 movement, float deltaTime) {
        (void)movement;
        (void)deltaTime;
    }

    void Actor::setPhysicsComponent(std::unique_ptr<PHYSICS::IPhysicsComponent> component) {
        physicsComponent = std::move(component);
    }

    PHYSICS::IPhysicsComponent* Actor::getPhysicsComponent() const {
        return physicsComponent.get();
    }

    bool Actor::hasPhysicsComponent() const {
        return physicsComponent != nullptr;
    }
}