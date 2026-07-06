//
// Created by Developer on [date].
//

#ifndef GLCPP_SIMPLECOLLISIONSYSTEM_H
#define GLCPP_SIMPLECOLLISIONSYSTEM_H

#include "ICollisionSystem.h"
#include "BVH.h"
#include "../Events/IEventSystem.h"
#include <vector>
#include <memory>

namespace SGE::PHYSICS {
    class SimpleCollisionSystem : public ICollisionSystem {
    private:
        std::shared_ptr<EVENTS::IEventSystem> eventSystem;
        
    public:
        SimpleCollisionSystem();
        ~SimpleCollisionSystem() override = default;
        
        // ICollisionSystem interface
        void addActor(std::shared_ptr<SGE::GAMEOBJECTS::Actor> actor);
        void removeActor(std::shared_ptr<SGE::GAMEOBJECTS::Actor> actor);
        void update(float deltaTime);
        std::vector<CollisionInfo> detectCollisions() override;
        bool checkCollision(const std::shared_ptr<SGE::GAMEOBJECTS::Actor>& actorA, 
                           const std::shared_ptr<SGE::GAMEOBJECTS::Actor>& actorB) override;
        void resolveCollision(CollisionInfo& collision) override;
        void applyImpulse(std::shared_ptr<SGE::GAMEOBJECTS::Actor> actor,
                          const glm::vec3& impulse) override;
        void updateSpatialPartitioning() override;
        std::vector<std::shared_ptr<SGE::GAMEOBJECTS::Actor>> getPotentialColliders(const std::shared_ptr<SGE::GAMEOBJECTS::Actor>& actor) override;
        void setElasticity(float elasticity) override;
        float getElasticity() const override;
        void setCollisionMethod(const std::string& method) override;
        std::string getCollisionMethod() const override;
        
        // Event system methods
        void setEventSystem(std::shared_ptr<EVENTS::IEventSystem> eventSys);
        
    private:
        BVH bvh;
        std::vector<std::shared_ptr<SGE::GAMEOBJECTS::Actor>> actors;
        std::string collisionMethod;
        float elasticity;
        
        // Helper methods
        void handleCollision(const std::shared_ptr<SGE::GAMEOBJECTS::Actor>& actorA,
                           const std::shared_ptr<SGE::GAMEOBJECTS::Actor>& actorB);
        void buildBVH();
        std::vector<std::shared_ptr<GAMEOBJECTS::Actor>> queryActorsInArea(const AABB& area);
        void handleCollisions(std::vector<CollisionInfo>& collisions);
        bool checkAABBCollision(const GAMEOBJECTS::Actor* actorA, const GAMEOBJECTS::Actor* actorB);
        bool checkSphereCollision(const GAMEOBJECTS::Actor* actorA, const GAMEOBJECTS::Actor* actorB);
    };
};

#endif // GLCPP_SIMPLECOLLISIONSYSTEM_H
