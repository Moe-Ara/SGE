//
// Created by Developer on [date].
//

#ifndef GLCPP_BVH_H
#define GLCPP_BVH_H

#include <vector>
#include <memory>
#include <span>
#include <glm/glm.hpp>
#include <entt/entt.hpp>
#include "../Core/ObjectPool.h"

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

    // A minimal snapshot of whatever the collision system needs to know about an
    // entity for a single frame's broad/narrow phase pass.
    struct SpatialItem {
        entt::entity entity;
        glm::vec3 position;
        float radius;
    };

    struct CollisionInfo {
        entt::entity entityA;
        entt::entity entityB;
        glm::vec3 contactPoint;
        glm::vec3 normal;
        float penetrationDepth;
    };

    class BVHNode {
    private:
        BVHNode* left{nullptr};
        BVHNode* right{nullptr};
        AABB bounds;
        const std::vector<SpatialItem>* items{nullptr};
        size_t firstItem{0};
        size_t itemCount{0};
        bool isLeaf;

    public:
        BVHNode();
        ~BVHNode() = default;

        void build(std::vector<SpatialItem>& itemsList,
                   size_t first,
                   size_t last,
                   CORE::ObjectPool<BVHNode>& pool);
        void query(const AABB& range, std::vector<SpatialItem>& result) const;
        void getCollisions(std::vector<CollisionInfo>& collisions) const;

        bool isLeafNode() const { return isLeaf; }
        const AABB& getBounds() const { return bounds; }
        std::span<const SpatialItem> getItems() const {
            return std::span<const SpatialItem>{*items}.subspan(firstItem, itemCount);
        }
        const BVHNode* getLeft() const { return left; }
        const BVHNode* getRight() const { return right; }

    };

    // Rebuilt from scratch every frame (call build()) from the current set of
    // collidable entities; there is no incremental insert/remove to go stale.
    class BVH {
    private:
        CORE::ObjectPool<BVHNode> nodePool{1};
        BVHNode* root{nullptr};
        std::vector<SpatialItem> workingItems;
        size_t itemCount{0};

    public:
        BVH();
        ~BVH() = default;

        void build(const std::vector<SpatialItem>& items);
        void query(const AABB& range, std::vector<SpatialItem>& result) const;
        void getCollisions(std::vector<CollisionInfo>& collisions) const;

        size_t size() const { return itemCount; }
        size_t allocatedNodeCount() const { return nodePool.capacity(); }
        size_t activeNodeCount() const { return nodePool.active(); }
    };
}

#endif // GLCPP_BVH_H
