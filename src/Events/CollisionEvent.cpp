//
// Created by Developer on [date].
//

#include "CollisionEvent.h"

namespace SGE::EVENTS {
    CollisionEvent::CollisionEvent(const std::string& name,
                                  entt::entity a,
                                  entt::entity b,
                                  const glm::vec3& point,
                                  const glm::vec3& normal,
                                  CollisionPhase phase)
        : eventType("collision"), eventName(name), entityA(a), entityB(b),
          collisionPoint(point), collisionNormal(normal), phase(phase) {}

    std::string CollisionEvent::getType() const {
        return eventType;
    }

    std::string CollisionEvent::getName() const {
        return eventName;
    }

    entt::entity CollisionEvent::getEntityA() const {
        return entityA;
    }

    entt::entity CollisionEvent::getEntityB() const {
        return entityB;
    }

    glm::vec3 CollisionEvent::getCollisionPoint() const {
        return collisionPoint;
    }

    glm::vec3 CollisionEvent::getCollisionNormal() const {
        return collisionNormal;
    }

    CollisionPhase CollisionEvent::getPhase() const {
        return phase;
    }
}
