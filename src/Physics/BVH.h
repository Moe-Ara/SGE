//
// Created by Developer on [date].
//

#ifndef GLCPP_BVH_H
#define GLCPP_BVH_H

#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include <entt/entt.hpp>

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
        std::unique_ptr<BVHNode> left;
        std::unique_ptr<BVHNode> right;
        AABB bounds;
        std::vector<SpatialItem> items;
        bool isLeaf;

    public:
        BVHNode();
        ~BVHNode() = default;

        void build(const std::vector<SpatialItem>& itemsList);
        void query(const AABB& range, std::vector<SpatialItem>& result) const;
        void getCollisions(std::vector<CollisionInfo>& collisions) const;

        bool isLeafNode() const { return isLeaf; }
        const AABB& getBounds() const { return bounds; }
        const std::vector<SpatialItem>& getItems() const { return items; }
        const BVHNode* getLeft() const { return left.get(); }
        const BVHNode* getRight() const { return right.get(); }
    };

    // Rebuilt from scratch every frame (call build()) from the current set of
    // collidable entities; there is no incremental insert/remove to go stale.
    class BVH {
    private:
        std::unique_ptr<BVHNode> root;
        size_t itemCount{0};

    public:
        BVH();
        ~BVH() = default;

        void build(const std::vector<SpatialItem>& items);
        void query(const AABB& range, std::vector<SpatialItem>& result) const;
        void getCollisions(std::vector<CollisionInfo>& collisions) const;

        size_t size() const { return itemCount; }
    };
}

#endif // GLCPP_BVH_H
