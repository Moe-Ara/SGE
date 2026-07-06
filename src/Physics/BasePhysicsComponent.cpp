//
// Created by Developer on [date].
//

#include "BasePhysicsComponent.h"

namespace SGE::PHYSICS {
    BasePhysicsComponent::BasePhysicsComponent()
        : position(0.0f), velocity(0.0f), acceleration(0.0f),
          force(0.0f), mass(1.0f), restitution(0.8f), isStaticState(false), enabled(true) {}
    
    void BasePhysicsComponent::update(float deltaTime) {
        if (isStaticState || !enabled) return;
        
        integrate(deltaTime);
    }
    
    void BasePhysicsComponent::setEnabled(bool enabledState) {
        enabled = enabledState;
    }
    
    bool BasePhysicsComponent::isEnabled() const {
        return enabled;
    }
    
    glm::vec3 BasePhysicsComponent::getPosition() const {
        return position;
    }
    
    void BasePhysicsComponent::setPosition(const glm::vec3& pos) {
        position = pos;
    }
    
    glm::vec3 BasePhysicsComponent::getVelocity() const {
        return velocity;
    }
    
    void BasePhysicsComponent::setVelocity(const glm::vec3& vel) {
        velocity = vel;
    }
    
    glm::vec3 BasePhysicsComponent::getAcceleration() const {
        return acceleration;
    }
    
    void BasePhysicsComponent::setAcceleration(const glm::vec3& acc) {
        acceleration = acc;
    }
    
    float BasePhysicsComponent::getMass() const {
        return mass;
    }
    
    void BasePhysicsComponent::setMass(float m) {
        mass = m;
    }
    
    float BasePhysicsComponent::getRestitution() const {
        return restitution;
    }
    
    void BasePhysicsComponent::setRestitution(float rest) {
        restitution = rest;
    }
    
    bool BasePhysicsComponent::isStatic() const {
        return isStaticState;
    }
    
    void BasePhysicsComponent::setStatic(bool staticObj) {
        isStaticState = staticObj;
    }
    
    void BasePhysicsComponent::addForce(const glm::vec3& f) {
        force += f;
    }
    
    void BasePhysicsComponent::clearForces() {
        force = glm::vec3(0.0f);
    }
    
    void BasePhysicsComponent::applyImpulse(const glm::vec3& impulse) {
        velocity += impulse / mass;
    }
    
    void BasePhysicsComponent::integrate(float deltaTime) {
        if (isStaticState || !enabled) return;
        
        // Calculate acceleration from force (F = ma => a = F/m)
        acceleration = force / mass;
        
        // Update velocity
        velocity += acceleration * deltaTime;
        
        // Update position
        position += velocity * deltaTime;
        
        // Clear forces for next frame
        clearForces();
    }
}