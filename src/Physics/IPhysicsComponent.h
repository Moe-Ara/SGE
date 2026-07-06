//
// Created by Developer on [date].
//

#ifndef GLCPP_IPHYSICSCOMPONENT_H
#define GLCPP_IPHYSICSCOMPONENT_H

#include <glm/glm.hpp>
#include <memory>

namespace SGE::PHYSICS {
    class IPhysicsComponent {
    public:
        virtual ~IPhysicsComponent() = default;
        
        // Physics properties
        virtual float getMass() const = 0;
        virtual void setMass(float mass) = 0;
        virtual void update(float deltaTime) = 0;
        virtual float getRestitution() const = 0;
        virtual void setRestitution(float restitution) = 0;

        virtual void applyImpulse(const glm::vec3& impulse) = 0;
        virtual glm::vec3 getPosition() const = 0;
        virtual void setPosition(const glm::vec3& position) = 0;
        
        virtual glm::vec3 getVelocity() const = 0;
        virtual void setVelocity(const glm::vec3& velocity) = 0;
        
        virtual glm::vec3 getAcceleration() const = 0;
        virtual void setAcceleration(const glm::vec3& acceleration) = 0;
        
        // Force operations
        virtual glm::vec3 getTotalForce() const = 0;
        virtual void addForce(const glm::vec3& force) = 0;
        virtual void clearForces() = 0;
        
        // Physics state
        virtual bool isStatic() const = 0;
        virtual void setStatic(bool isStatic) = 0;
        
        virtual bool isEnabled() const = 0;
        virtual void setEnabled(bool enabled) = 0;
        
        // Integration
        virtual void integrate(float deltaTime) = 0;
    };
}

#endif //GLCPP_IPHYSICSCOMPONENT_H