//
// Created by Developer on [date].
//

#include "PhysicsEngine.h"
#include "SimpleCollisionSystem.h"
#include "BasePhysicsComponent.h"
#include "../GameObjects/Actor.h"
#include "../Events/CollisionEvent.h"

namespace SGE::PHYSICS {
    PhysicsEngine::PhysicsEngine() 
        : isInitialized(false) {
        collisionSystem = std::make_unique<SimpleCollisionSystem>();
    }
    
    void PhysicsEngine::initialize() {
        if (!isInitialized) {
            isInitialized = true;
        }
    }
    
    void PhysicsEngine::setEventSystem(std::shared_ptr<EVENTS::IEventSystem> eventSys) {
        eventSystem = eventSys;
    }
    
    void PhysicsEngine::update(float deltaTime) {
        if (!isInitialized) {
            initialize();
        }
        
        // Apply forces to all actors
        applyForces();
        
        // Integrate positions
        integratePositions(deltaTime);
        
        // Detect and handle collisions
        auto collisions = detectCollisions();
        
        // Publish collision events
        if (eventSystem && !collisions.empty()) {
            for (const auto& collision : collisions) {
                // Create and publish collision event
                auto compA = std::shared_ptr<PHYSICS::BasePhysicsComponent>(
                    dynamic_cast<PHYSICS::BasePhysicsComponent*>(collision.actorA->getPhysicsComponent()));
                auto compB = std::shared_ptr<PHYSICS::BasePhysicsComponent>(
                    dynamic_cast<PHYSICS::BasePhysicsComponent*>(collision.actorB->getPhysicsComponent()));
                auto event = std::make_shared<EVENTS::CollisionEvent>(
                    "CollisionEvent",
                    compA,
                    compB,
                    glm::vec3(0.0f), // collision point
                    glm::vec3(0.0f)  // collision normal
                );
                
                eventSystem->publish(*event);
            }
        }
    }
    
    void PhysicsEngine::addActor(std::shared_ptr<SGE::GAMEOBJECTS::Actor> actor) {
        actors.push_back(actor);
        collisionSystem->addActor(actor);
    }
    
    void PhysicsEngine::removeActor(std::shared_ptr<SGE::GAMEOBJECTS::Actor> actor) {
        // Remove from actors list
        auto it = std::find(actors.begin(), actors.end(), actor);
        if (it != actors.end()) {
            actors.erase(it);
        }
        
        collisionSystem->removeActor(actor);
    }
    
    bool PhysicsEngine::checkCollision(const std::shared_ptr<SGE::GAMEOBJECTS::Actor>& actorA,
                                       const std::shared_ptr<SGE::GAMEOBJECTS::Actor>& actorB) {
        return collisionSystem ? collisionSystem->checkCollision(actorA, actorB) : false;
    }

    void PhysicsEngine::resolveCollision(CollisionInfo& collision) {
        if (collisionSystem) {
            collisionSystem->resolveCollision(collision);
        }
    }

    void PhysicsEngine::applyImpulse(std::shared_ptr<SGE::GAMEOBJECTS::Actor> actor,
                                     const glm::vec3& impulse) {
        if (collisionSystem) {
            collisionSystem->applyImpulse(actor, impulse);
        }
    }

    void PhysicsEngine::updateSpatialPartitioning() {
        if (collisionSystem) {
            collisionSystem->updateSpatialPartitioning();
        }
    }

    std::vector<std::shared_ptr<SGE::GAMEOBJECTS::Actor>> PhysicsEngine::getPotentialColliders(const std::shared_ptr<SGE::GAMEOBJECTS::Actor>& actor) {
        return collisionSystem ? collisionSystem->getPotentialColliders(actor) : std::vector<std::shared_ptr<SGE::GAMEOBJECTS::Actor>>{};
    }

    void PhysicsEngine::setElasticity(float elasticity) {
        (void)elasticity;
    }

    float PhysicsEngine::getElasticity() const {
        return 0.8f;
    }

    std::vector<CollisionInfo> PhysicsEngine::detectCollisions() {
        return collisionSystem ? collisionSystem->detectCollisions() : std::vector<CollisionInfo>{};
    }
    
    void PhysicsEngine::setCollisionMethod(const std::string& method) {
        if (collisionSystem) {
            collisionSystem->setCollisionMethod(method);
        }
    }
    
    std::string PhysicsEngine::getCollisionMethod() const {
        return collisionSystem ? collisionSystem->getCollisionMethod() : "";
    }
    
    void PhysicsEngine::stepSimulation(float deltaTime) {
        // Simple physics simulation step
        applyForces();
        integratePositions(deltaTime);
    }
    
    void PhysicsEngine::applyForces() {
        for (const auto& actor : actors) {
            auto phys = dynamic_cast<PHYSICS::BasePhysicsComponent*>(actor->getPhysicsComponent());
            if (phys) {
                // Apply gravity
                glm::vec3 gravity(0.0f, -9.81f, 0.0f);
                phys->addForce(gravity * phys->getMass());
            }
        }
    }
    
    void PhysicsEngine::integratePositions(float deltaTime) {
        for (const auto& actor : actors) {
            auto phys = dynamic_cast<PHYSICS::BasePhysicsComponent*>(actor->getPhysicsComponent());
            if (phys) {
                // Simple integration
                glm::vec3 velocity = phys->getVelocity();
                glm::vec3 position = actor->getTransform().getPosition();
                
                // Update position based on velocity
                position += velocity * deltaTime;
                
                // Update actor transform
                auto transform = actor->getTransform();
                transform.setPosition(position);
                actor->setTransform(transform);
            }
        }
    }
}