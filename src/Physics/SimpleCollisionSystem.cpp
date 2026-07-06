//
// Created by Developer on [date].
//

#include "SimpleCollisionSystem.h"
#include "BasePhysicsComponent.h"
#include "../GameObjects/Actor.h"
#include "../Events/CollisionEvent.h"
#include <algorithm>

namespace SGE::PHYSICS {
    SimpleCollisionSystem::SimpleCollisionSystem() : ICollisionSystem(), elasticity(0.8f), collisionMethod("BVH") {}
    
    void SimpleCollisionSystem::addActor(std::shared_ptr<GAMEOBJECTS::Actor> actor) {
        actors.push_back(actor);
        bvh.insert(actor);
    }
    
    void SimpleCollisionSystem::removeActor(std::shared_ptr<GAMEOBJECTS::Actor> actor) {
        // Remove from actors list
        auto it = std::find(actors.begin(), actors.end(), actor);
        if (it != actors.end()) {
            actors.erase(it);
        }
        
        bvh.remove(actor);
    }
    
    void SimpleCollisionSystem::update(float deltaTime) {
        // Update the BVH structure for current frame
        bvh.update();
    }
    
    std::vector<CollisionInfo> SimpleCollisionSystem::detectCollisions() {
        std::vector<CollisionInfo> collisions;
        
        // Use BVH to efficiently find potential collisions
        bvh.getCollisions(collisions);
        
        return collisions;
    }
    
    bool SimpleCollisionSystem::checkCollision(const std::shared_ptr<SGE::GAMEOBJECTS::Actor>& actorA,
                                               const std::shared_ptr<SGE::GAMEOBJECTS::Actor>& actorB) {
        return checkSphereCollision(actorA.get(), actorB.get());
    }

    void SimpleCollisionSystem::resolveCollision(CollisionInfo& collision) {
        // Simple elastic collision response
        auto actorA = collision.actorA;
        auto actorB = collision.actorB;

        if (!actorA || !actorB) return;

        // Get physics components if they exist
        auto physA = dynamic_cast<PHYSICS::BasePhysicsComponent*>(actorA->getPhysicsComponent());
        auto physB = dynamic_cast<PHYSICS::BasePhysicsComponent*>(actorB->getPhysicsComponent());

        if (physA && physB) {
            glm::vec3 posA = actorA->getTransform().getPosition();
            glm::vec3 posB = actorB->getTransform().getPosition();

            glm::vec3 normal = glm::normalize(posB - posA);
            float restitution = 0.8f;
            (void)restitution;

            physA->addForce(normal * 10.0f);
            physB->addForce(-normal * 10.0f);
        }
    }

    void SimpleCollisionSystem::applyImpulse(std::shared_ptr<SGE::GAMEOBJECTS::Actor> actor,
                                             const glm::vec3& impulse) {
        if (actor) {
            auto phys = dynamic_cast<PHYSICS::BasePhysicsComponent*>(actor->getPhysicsComponent());
            if (phys) {
                phys->addForce(impulse);
            }
        }
    }

    void SimpleCollisionSystem::updateSpatialPartitioning() {
        bvh.update();
    }

    std::vector<std::shared_ptr<SGE::GAMEOBJECTS::Actor>> SimpleCollisionSystem::getPotentialColliders(const std::shared_ptr<SGE::GAMEOBJECTS::Actor>& actor) {
        std::vector<std::shared_ptr<SGE::GAMEOBJECTS::Actor>> result;
        if (actor) {
            AABB area;
            area.min = actor->getTransform().getPosition() - glm::vec3(1.0f);
            area.max = actor->getTransform().getPosition() + glm::vec3(1.0f);
            bvh.query(area, result);
        }
        return result;
    }

    void SimpleCollisionSystem::setElasticity(float elasticityValue) {
        elasticity = elasticityValue;
    }

    float SimpleCollisionSystem::getElasticity() const {
        return elasticity;
    }

    void SimpleCollisionSystem::setCollisionMethod(const std::string& method) {
        collisionMethod = method;
    }
    
    std::string SimpleCollisionSystem::getCollisionMethod() const {
        return collisionMethod;
    }
    
    void SimpleCollisionSystem::buildBVH() {
        // Build the BVH from current actors
        for (const auto& actor : actors) {
            bvh.insert(actor);
        }
    }
    
    std::vector<std::shared_ptr<GAMEOBJECTS::Actor>> SimpleCollisionSystem::queryActorsInArea(const AABB& area) {
        std::vector<std::shared_ptr<GAMEOBJECTS::Actor>> result;
        bvh.query(area, result);
        return result;
    }
    
    void SimpleCollisionSystem::handleCollisions(std::vector<CollisionInfo>& collisions) {
        for (auto& collision : collisions) {
            resolveCollision(collision);
        }
    }
    
    bool SimpleCollisionSystem::checkAABBCollision(const GAMEOBJECTS::Actor* actorA, const GAMEOBJECTS::Actor* actorB) {
        if (!actorA || !actorB) return false;
        
        // Get bounding boxes for both actors
        auto transformA = actorA->getTransform();
        auto transformB = actorB->getTransform();
        
        glm::vec3 posA = transformA.getPosition();
        glm::vec3 posB = transformB.getPosition();
        
        // Simple sphere-based collision detection (for now)
        float distance = glm::distance(posA, posB);
        float radiusA = 0.5f;
        float radiusB = 0.5f;
        
        return distance < (radiusA + radiusB);
    }
    
    bool SimpleCollisionSystem::checkSphereCollision(const GAMEOBJECTS::Actor* actorA, const GAMEOBJECTS::Actor* actorB) {
        if (!actorA || !actorB) return false;
        
        auto transformA = actorA->getTransform();
        auto transformB = actorB->getTransform();
        
        glm::vec3 posA = transformA.getPosition();
        glm::vec3 posB = transformB.getPosition();
        
        float distance = glm::distance(posA, posB);
        float radiusA = 0.5f;
        float radiusB = 0.5f;
        
        return distance < (radiusA + radiusB);
    }
    
    void SimpleCollisionSystem::setEventSystem(std::shared_ptr<EVENTS::IEventSystem> eventSys) {
        eventSystem = eventSys;
    }

    void SimpleCollisionSystem::handleCollision(const std::shared_ptr<SGE::GAMEOBJECTS::Actor>& actorA,
                                               const std::shared_ptr<SGE::GAMEOBJECTS::Actor>& actorB) {
        // This method can be extended to handle specific collision logic
        // For now, it's just a placeholder for potential future implementation
    }
}
