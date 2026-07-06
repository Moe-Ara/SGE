//
// Created by Developer on [date].
//

#ifndef GLCPP_PHYSICSENGINE_H
#define GLCPP_PHYSICSENGINE_H

#include "ICollisionSystem.h"
#include "SimpleCollisionSystem.h"
#include "../Events/IEventSystem.h"
#include <memory>
#include <vector>

namespace SGE::PHYSICS {
    class PhysicsEngine : public ICollisionSystem {
    private:
        std::unique_ptr<SimpleCollisionSystem> collisionSystem;
        std::shared_ptr<EVENTS::IEventSystem> eventSystem;
        bool isInitialized;
        
    public:
        PhysicsEngine();
        ~PhysicsEngine() override = default;
        
        // ICollisionSystem interface
        void addActor(std::shared_ptr<SGE::GAMEOBJECTS::Actor> actor);
        void removeActor(std::shared_ptr<SGE::GAMEOBJECTS::Actor> actor);
        void update(float deltaTime);
        bool checkCollision(const std::shared_ptr<SGE::GAMEOBJECTS::Actor>& actorA,
                            const std::shared_ptr<SGE::GAMEOBJECTS::Actor>& actorB) override;
        void resolveCollision(CollisionInfo& collision) override;
        void applyImpulse(std::shared_ptr<SGE::GAMEOBJECTS::Actor> actor,
                          const glm::vec3& impulse) override;
        void updateSpatialPartitioning() override;
        std::vector<std::shared_ptr<SGE::GAMEOBJECTS::Actor>> getPotentialColliders(const std::shared_ptr<SGE::GAMEOBJECTS::Actor>& actor) override;
        void setElasticity(float elasticity) override;
        float getElasticity() const override;
        std::vector<CollisionInfo> detectCollisions() override;
        void setCollisionMethod(const std::string& method) override;
        std::string getCollisionMethod() const override;
        
        // Physics engine specific methods
        void initialize();
        void stepSimulation(float deltaTime);
        void applyForces();
        void integratePositions(float deltaTime);
        
        // Event system methods
        void setEventSystem(std::shared_ptr<EVENTS::IEventSystem> eventSys);
        
    private:
        std::vector<std::shared_ptr<SGE::GAMEOBJECTS::Actor>> actors;
    };
};

#endif // GLCPP_PHYSICSENGINE_H
