//
// Created by Developer on [date].
//

#ifndef GLCPP_COLLISIONEVENT_H
#define GLCPP_COLLISIONEVENT_H

#include "IEvent.h"
#include "../Physics/PhysicsEngine.h"
#include <string>

namespace SGE::PHYSICS {
    class BasePhysicsComponent;
}

namespace SGE::EVENTS {
    class CollisionEvent : public IEvent {
    private:
        std::string eventType;
        std::string eventName;
        std::shared_ptr<PHYSICS::BasePhysicsComponent> componentA;
        std::shared_ptr<PHYSICS::BasePhysicsComponent> componentB;
        glm::vec3 collisionPoint;
        glm::vec3 collisionNormal;
        
    public:
        CollisionEvent(const std::string& name,
                      const std::shared_ptr<PHYSICS::BasePhysicsComponent>& compA,
                      const std::shared_ptr<PHYSICS::BasePhysicsComponent>& compB,
                      const glm::vec3& point,
                      const glm::vec3& normal);
        
        ~CollisionEvent() override = default;
        
        // IEvent interface
        std::string getType() const override;
        std::string getName() const override;
        
        // Collision event specific methods
        std::shared_ptr<PHYSICS::BasePhysicsComponent> getComponentA() const;
        std::shared_ptr<PHYSICS::BasePhysicsComponent> getComponentB() const;
        glm::vec3 getCollisionPoint() const;
        glm::vec3 getCollisionNormal() const;
    };
};

#endif // GLCPP_COLLISIONEVENT_H