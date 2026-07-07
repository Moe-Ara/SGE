//
// Created by Developer on [date].
//

#ifndef GLCPP_COLLISIONEVENT_H
#define GLCPP_COLLISIONEVENT_H

#include "IEvent.h"
#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <string>

namespace SGE::EVENTS {
    class CollisionEvent : public IEvent {
    private:
        std::string eventType;
        std::string eventName;
        entt::entity entityA;
        entt::entity entityB;
        glm::vec3 collisionPoint;
        glm::vec3 collisionNormal;

    public:
        CollisionEvent(const std::string& name,
                      entt::entity a,
                      entt::entity b,
                      const glm::vec3& point,
                      const glm::vec3& normal);

        ~CollisionEvent() override = default;

        // IEvent interface
        std::string getType() const override;
        std::string getName() const override;

        // Collision event specific methods
        entt::entity getEntityA() const;
        entt::entity getEntityB() const;
        glm::vec3 getCollisionPoint() const;
        glm::vec3 getCollisionNormal() const;
    };
};

#endif // GLCPP_COLLISIONEVENT_H
