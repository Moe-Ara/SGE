//
// Created by Developer on [date].
//

#ifndef GLCPP_BASEPHYSICSCOMPONENT_H
#define GLCPP_BASEPHYSICSCOMPONENT_H

#include "IPhysicsComponent.h"
#include <glm/glm.hpp>

namespace SGE::PHYSICS {
    class BasePhysicsComponent : public IPhysicsComponent {
    private:
        glm::vec3 position;
        glm::vec3 velocity;
        glm::vec3 acceleration;
        glm::vec3 force;
        float mass;
        float restitution; // Bounciness
        bool isStaticState;
        bool enabled;
    public:
        BasePhysicsComponent();
        ~BasePhysicsComponent() override = default;
        
        // IPhysicsComponent interface
        void update(float deltaTime) override;
        void setEnabled(bool enabled) override;
        bool isEnabled() const override;
        
        // Physics properties
        glm::vec3 getPosition() const override;
        void setPosition(const glm::vec3& pos) override;
        glm::vec3 getVelocity() const override;
        void setVelocity(const glm::vec3& vel) override;
        glm::vec3 getAcceleration() const override;
        void setAcceleration(const glm::vec3& acc) override;
        float getMass() const override;
        void setMass(float mass) override;
        float getRestitution() const override;
        void setRestitution(float restitution) override;
        bool isStatic() const override;
        void setStatic(bool isStatic) override;

        // Physics operations
        void addForce(const glm::vec3& f) override;
        void clearForces() override;
        void applyImpulse(const glm::vec3& impulse) override;

    private:
        void integrate(float deltaTime);
    };
};
#endif // GLCPP_BASEPHYSICSCOMPONENT_H
