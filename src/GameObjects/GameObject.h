//
// Created by Mohamad on 11/07/2024.
//

#ifndef GLCPP_GAMEOBJECT_H
#define GLCPP_GAMEOBJECT_H

#include <string>
#include <memory>
#include <glm/glm.hpp>
#include "./Transform.h"
#include "Model.h"
#include "../Physics/IPhysicsComponent.h"
#include "../Events/IEventSystem.h"
namespace SGE::GAMEOBJECTS {
class GameObject {
    private:
        std::string id;
        std::string name;
        Transform transform;
        std::shared_ptr<Model> model;
        std::shared_ptr<PHYSICS::IPhysicsComponent> physicsComponent;
        std::shared_ptr<EVENTS::IEventSystem> eventSystem;

    public:
        GameObject(Transform transform, std::shared_ptr<Model> model);
        virtual ~GameObject() = default;

        // Getters and setters
        std::string getId() const;
        std::string getName() const;
        Transform getTransform() const;
        void setTransform(const Transform& t);
        Transform& getTransformRef();
        std::shared_ptr<PHYSICS::IPhysicsComponent> getPhysicsComponent() const;
        void setPhysicsComponent(std::shared_ptr<PHYSICS::IPhysicsComponent> component);

        // Event system methods
        void setEventSystem(std::shared_ptr<EVENTS::IEventSystem> eventSys);
        std::shared_ptr<EVENTS::IEventSystem> getEventSystem() const;

        // Update method
    virtual void update(float deltaTime);
    virtual void render();
    virtual glm::vec3 getColor() const;
};
};

#endif // GLCPP_GAMEOBJECT_H

