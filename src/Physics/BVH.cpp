//
// Created by Developer on [date].
//

#include "BVH.h"
#include <algorithm>
#include <limits>

namespace SGE::PHYSICS {
    // AABB Implementation
    bool AABB::intersects(const AABB& other) const {
        return min.x <= other.max.x && max.x >= other.min.x &&
               min.y <= other.max.y && max.y >= other.min.y &&
               min.z <= other.max.z && max.z >= other.min.z;
    }
    
    float AABB::getVolume() const {
        glm::vec3 size = max - min;
        return size.x * size.y * size.z;
    }
    
    glm::vec3 AABB::getCenter() const {
        return (min + max) * 0.5f;
    }
    
    // BVHNode Implementation
    BVHNode::BVHNode() : isLeaf(false) {}
    
    void BVHNode::insert(std::shared_ptr<GAMEOBJECTS::Actor> actor) {
        if (isLeaf) {
            actors.push_back(actor);
            return;
        }
        
        // For simplicity, we'll just add to the first child for now
        // In a full implementation, this would split based on cost function
        if (left) left->insert(actor);
        else if (right) right->insert(actor);
    }
    
    void BVHNode::build(const std::vector<std::shared_ptr<GAMEOBJECTS::Actor>>& actorsList) {
        actors = actorsList;
        
        if (actors.empty()) {
            isLeaf = true;
            return;
        }
        
        // Calculate bounding box for all actors
        AABB totalBounds;
        bool first = true;
        
        for (const auto& actor : actors) {
            // Get actor bounds (this would need to be more sophisticated)
            // For now, we'll use a simple approach based on transform
            auto transform = actor->getTransform();
            glm::vec3 position = transform.getMTranslation();
            
            if (first) {
                totalBounds.min = position;
                totalBounds.max = position;
                first = false;
            } else {
                totalBounds.min = glm::min(totalBounds.min, position);
                totalBounds.max = glm::max(totalBounds.max, position);
            }
        }
        
        bounds = totalBounds;
        isLeaf = actors.size() <= 4; // Simple leaf threshold
        
        if (!isLeaf && actors.size() > 0) {
            // Simple splitting - divide by x-axis for now
            float split = (totalBounds.min.x + totalBounds.max.x) * 0.5f;
            
            std::vector<std::shared_ptr<GAMEOBJECTS::Actor>> leftActors, rightActors;
            
            for (const auto& actor : actors) {
                auto transform = actor->getTransform();
                glm::vec3 position = transform.getMTranslation();
                
                if (position.x <= split) {
                    leftActors.push_back(actor);
                } else {
                    rightActors.push_back(actor);
                }
            }
            
            if (!leftActors.empty()) {
                left = std::make_unique<BVHNode>();
                left->build(leftActors);
            }
            
            if (!rightActors.empty()) {
                right = std::make_unique<BVHNode>();
                right->build(rightActors);
            }
        }
    }
    
    void BVHNode::query(const AABB& range, std::vector<std::shared_ptr<GAMEOBJECTS::Actor>>& result) const {
        if (!bounds.intersects(range)) {
            return;
        }
        
        if (isLeaf) {
            for (const auto& actor : actors) {
                result.push_back(actor);
            }
            return;
        }
        
        if (left) left->query(range, result);
        if (right) right->query(range, result);
    }
    
    void BVHNode::getCollisions(std::vector<CollisionInfo>& collisions) const {
        if (isLeaf) {
            // Check pairwise collisions within this leaf
            for (size_t i = 0; i < actors.size(); ++i) {
                for (size_t j = i + 1; j < actors.size(); ++j) {
                    auto actorA = actors[i];
                    auto actorB = actors[j];
                    
                    // Simple sphere-sphere collision detection
                    glm::vec3 posA = actorA->getTransform().getMTranslation();
                    glm::vec3 posB = actorB->getTransform().getMTranslation();
                    float distance = glm::distance(posA, posB);
                    
                    // Assume a simple radius for collision detection
                    float radiusA = 0.5f;
                    float radiusB = 0.5f;
                    
                    if (distance < radiusA + radiusB) {
                        CollisionInfo info;
                        info.actorA = actorA;
                        info.actorB = actorB;
                        info.penetrationDepth = distance;
                        collisions.push_back(info);
                    }
                }
            }
            return;
        }
        
        // Recursively check children
        if (left) left->getCollisions(collisions);
        if (right) right->getCollisions(collisions);
    }
    
    // BVH Implementation
    BVH::BVH() {
        root = std::make_unique<BVHNode>();
    }
    
    void BVH::insert(std::shared_ptr<GAMEOBJECTS::Actor> actor) {
        allActors.push_back(actor);
        root->insert(actor);
    }
    
    void BVH::remove(std::shared_ptr<GAMEOBJECTS::Actor> actor) {
        // Remove from all actors list
        auto it = std::find(allActors.begin(), allActors.end(), actor);
        if (it != allActors.end()) {
            allActors.erase(it);
        }
        
        // In a real implementation, we'd rebuild the tree or mark for removal
        // For now, we'll just mark as needing update
    }
    
    void BVH::update() {
        // Rebuild the tree with current actors
        if (!allActors.empty()) {
            root = std::make_unique<BVHNode>();
            root->build(allActors);
        }
    }
    
    void BVH::query(const AABB& range, std::vector<std::shared_ptr<GAMEOBJECTS::Actor>>& result) const {
        if (root) {
            root->query(range, result);
        }
    }
    
    void BVH::getCollisions(std::vector<CollisionInfo>& collisions) const {
        if (root) {
            root->getCollisions(collisions);
        }
    }
}