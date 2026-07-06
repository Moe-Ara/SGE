//
// Created by Developer on [date].
//

#ifndef GLCPP_BVH_H
#define GLCPP_BVH_H

#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include "../GameObjects/Actor.h"
#include "ICollisionSystem.h"

namespace SGE::PHYSICS {
    struct AABB {
        glm::vec3 min;
        glm::vec3 max;
        
        AABB() : min(0.0f), max(0.0f) {}
        AABB(const glm::vec3& minPoint, const glm::vec3& maxPoint) 
            : min(minPoint), max(maxPoint) {}
        
        bool intersects(const AABB& other) const;
        float getVolume() const;
        glm::vec3 getCenter() const;
    };
    
    class BVHNode {
    private:
        std::unique_ptr<BVHNode> left;
        std::unique_ptr<BVHNode> right;
        AABB bounds;
        std::vector<std::shared_ptr<GAMEOBJECTS::Actor>> actors;
        bool isLeaf;
        
    public:
        BVHNode();
        ~BVHNode() = default;
        
        void insert(std::shared_ptr<GAMEOBJECTS::Actor> actor);
        void build(const std::vector<std::shared_ptr<GAMEOBJECTS::Actor>>& actors);
        void query(const AABB& range, std::vector<std::shared_ptr<GAMEOBJECTS::Actor>>& result) const;
        void getCollisions(std::vector<CollisionInfo>& collisions) const;
        
        bool isLeafNode() const { return isLeaf; }
        const AABB& getBounds() const { return bounds; }
        const std::vector<std::shared_ptr<GAMEOBJECTS::Actor>>& getActors() const { return actors; }
    };
    
    class BVH {
    private:
        std::unique_ptr<BVHNode> root;
        std::vector<std::shared_ptr<GAMEOBJECTS::Actor>> allActors;
        
    public:
        BVH();
        ~BVH() = default;
        
        void insert(std::shared_ptr<GAMEOBJECTS::Actor> actor);
        void remove(std::shared_ptr<GAMEOBJECTS::Actor> actor);
        void update();
        void query(const AABB& range, std::vector<std::shared_ptr<GAMEOBJECTS::Actor>>& result) const;
        void getCollisions(std::vector<CollisionInfo>& collisions) const;
        
        size_t size() const { return allActors.size(); }
    };
}

#endif // GLCPP_BVH_H