//
// Created by Developer on [date].
//

#ifndef GLCPP_ICOLLISIONSYSTEM_H
#define GLCPP_ICOLLISIONSYSTEM_H

#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include "../GameObjects/Actor.h"

namespace SGE::PHYSICS {
    struct CollisionInfo {
        std::shared_ptr<SGE::GAMEOBJECTS::Actor> actorA;
        std::shared_ptr<SGE::GAMEOBJECTS::Actor> actorB;
        glm::vec3 contactPoint;
        glm::vec3 normal;
        float penetrationDepth;
    };
    
    class ICollisionSystem {
    public:
        virtual ~ICollisionSystem() = default;
        
        // Detection
        virtual std::vector<CollisionInfo> detectCollisions() = 0;
        virtual bool checkCollision(const std::shared_ptr<SGE::GAMEOBJECTS::Actor>& actorA, 
                                   const std::shared_ptr<SGE::GAMEOBJECTS::Actor>& actorB) = 0;
        
        // Collision response
        virtual void resolveCollision(CollisionInfo& collision) = 0;
        virtual void applyImpulse(std::shared_ptr<SGE::GAMEOBJECTS::Actor> actor, 
                                 const glm::vec3& impulse) = 0;
        
        // Spatial partitioning
        virtual void updateSpatialPartitioning() = 0;
        virtual std::vector<std::shared_ptr<SGE::GAMEOBJECTS::Actor>> getPotentialColliders(const std::shared_ptr<SGE::GAMEOBJECTS::Actor>& actor) = 0;
        
        // Configuration
        virtual void setCollisionMethod(const std::string& method) = 0;
        virtual std::string getCollisionMethod() const = 0;
        virtual void setElasticity(float elasticity) = 0;
        virtual float getElasticity() const = 0;
    };
}

#endif //GLCPP_ICOLLISIONSYSTEM_H