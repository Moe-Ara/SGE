//
// Created by Mohamad on 12/06/2024.
//

#ifndef GLCPP_ACTOR_H
#define GLCPP_ACTOR_H

#include "GameObject.h"
#include "Transform.h"
#include "../Physics/IPhysicsComponent.h"
#include <memory>
#include <glm/glm.hpp>
#include "../Graphics/Shader.h"

namespace SGE::GAMEOBJECTS {
    class Actor : public GameObject {

    public:
        Actor(long ID, Transform transform,
              std::shared_ptr<Model> model);

        virtual ~Actor() = default;

        virtual void move(glm::vec3 movement, float deltaTime);
        virtual void update(float deltaTime);
        virtual void applyForce(const glm::vec3& force);

        // Physics interface
        virtual void setPhysicsComponent(std::unique_ptr<PHYSICS::IPhysicsComponent> component);
        virtual PHYSICS::IPhysicsComponent* getPhysicsComponent() const;
        virtual bool hasPhysicsComponent() const;

        virtual void render(const std::shared_ptr<SGE::GRAPHICS::Shader>& shader) = 0;
        glm::vec3 getPosition() const { return position; }
        void setPosition(const glm::vec3& position) { this->position = position; }

    protected:
        std::unique_ptr<PHYSICS::IPhysicsComponent> physicsComponent;
        glm::vec3 position;
        long ID;
        float mass;
        glm::vec3 velocity;
        glm::vec3 accumulatedForces;
    };
};
#endif //GLCPP_ACTOR_H

