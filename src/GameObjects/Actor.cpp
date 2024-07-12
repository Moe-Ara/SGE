//
// Created by Mohamad on 11/07/2024.
//
#include "Actor.h"

#include <utility>

namespace SGE::GAMEOBJECTS {

    GAMEOBJECTS::Actor::Actor(long ID, Transform transform,
                              std::shared_ptr<Model> model) : ID(ID), mass(1.0f), velocity(0.0f),
                                                              accumulatedForces(0.0f),
                                                              GameObject(transform, std::move(model)) {

    }


    void GAMEOBJECTS::Actor::applyForce(const glm::vec3 &force) {
        accumulatedForces += force;
    }

    void GAMEOBJECTS::Actor::update(float deltaTime) {
        glm::vec3 acceleration = accumulatedForces / mass;
        velocity += acceleration * deltaTime;
        move(velocity, deltaTime);
        // Reset forces after applying them
        accumulatedForces = glm::vec3(0.0f);
    }

    void Actor::move(glm::vec3 movement, float deltaTime) {

    }
}