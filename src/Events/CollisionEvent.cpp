//
// Created by Developer on [date].
//

#include "CollisionEvent.h"

namespace SGE::EVENTS {
    CollisionEvent::CollisionEvent(const std::string& name,
                                  const std::shared_ptr<PHYSICS::BasePhysicsComponent>& compA,
                                  const std::shared_ptr<PHYSICS::BasePhysicsComponent>& compB,
                                  const glm::vec3& point,
                                  const glm::vec3& normal)
        : eventType("collision"), eventName(name), componentA(compA), componentB(compB),
          collisionPoint(point), collisionNormal(normal) {}
    
    std::string CollisionEvent::getType() const {
        return eventType;
    }
    
    std::string CollisionEvent::getName() const {
        return eventName;
    }
    
    std::shared_ptr<PHYSICS::BasePhysicsComponent> CollisionEvent::getComponentA() const {
        return componentA;
    }
    
    std::shared_ptr<PHYSICS::BasePhysicsComponent> CollisionEvent::getComponentB() const {
        return componentB;
    }
    
    glm::vec3 CollisionEvent::getCollisionPoint() const {
        return collisionPoint;
    }
    
    glm::vec3 CollisionEvent::getCollisionNormal() const {
        return collisionNormal;
    }
}