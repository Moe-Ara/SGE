//
// Created by Developer on [date].
//

#include "BVH.h"
#include <algorithm>
#include <limits>

namespace SGE::PHYSICS {
    namespace {
        void testPair(const SpatialItem& itemA, const SpatialItem& itemB, std::vector<CollisionInfo>& collisions) {
            const float distance = glm::distance(itemA.position, itemB.position);
            if (distance < itemA.radius + itemB.radius) {
                CollisionInfo info;
                info.entityA = itemA.entity;
                info.entityB = itemB.entity;
                info.normal = distance > 0.0f
                    ? (itemB.position - itemA.position) / distance
                    : glm::vec3(0.0f, 1.0f, 0.0f);
                info.penetrationDepth = (itemA.radius + itemB.radius) - distance;
                const glm::vec3 pointOnA = itemA.position + info.normal * itemA.radius;
                const glm::vec3 pointOnB = itemB.position - info.normal * itemB.radius;
                info.contactPoint = (pointOnA + pointOnB) * 0.5f;
                collisions.push_back(info);
            }
        }

        // Dual-tree traversal: checks every pair of items where one comes from
        // subtree `a` and the other from subtree `b`, pruned by bounds overlap.
        // This is what makes cross-partition collisions (two items on opposite
        // sides of a BVH split, but close enough to overlap) actually get found.
        void crossCheck(const BVHNode& a, const BVHNode& b, std::vector<CollisionInfo>& collisions) {
            if (!a.getBounds().intersects(b.getBounds())) {
                return;
            }

            if (a.isLeafNode() && b.isLeafNode()) {
                for (const auto& itemA : a.getItems()) {
                    for (const auto& itemB : b.getItems()) {
                        testPair(itemA, itemB, collisions);
                    }
                }
            } else if (a.isLeafNode()) {
                if (b.getLeft()) crossCheck(a, *b.getLeft(), collisions);
                if (b.getRight()) crossCheck(a, *b.getRight(), collisions);
            } else if (b.isLeafNode()) {
                if (a.getLeft()) crossCheck(*a.getLeft(), b, collisions);
                if (a.getRight()) crossCheck(*a.getRight(), b, collisions);
            } else {
                if (a.getLeft() && b.getLeft()) crossCheck(*a.getLeft(), *b.getLeft(), collisions);
                if (a.getLeft() && b.getRight()) crossCheck(*a.getLeft(), *b.getRight(), collisions);
                if (a.getRight() && b.getLeft()) crossCheck(*a.getRight(), *b.getLeft(), collisions);
                if (a.getRight() && b.getRight()) crossCheck(*a.getRight(), *b.getRight(), collisions);
            }
        }
    }

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

    void BVHNode::build(std::vector<SpatialItem>& itemsList,
                        size_t first,
                        size_t last,
                        CORE::ObjectPool<BVHNode>& pool) {
        items = &itemsList;
        firstItem = first;
        itemCount = last - first;
        left = nullptr;
        right = nullptr;

        if (itemCount == 0u) {
            isLeaf = true;
            bounds = AABB{};
            return;
        }

        // Bounds must include full sphere extents or cross-node overlaps can be pruned.
        AABB totalBounds;
        bool firstBound = true;

        for (size_t index = first; index < last; ++index) {
            const auto& item = itemsList[index];
            const glm::vec3 radius{item.radius};
            const glm::vec3 itemMin = item.position - radius;
            const glm::vec3 itemMax = item.position + radius;
            if (firstBound) {
                totalBounds.min = itemMin;
                totalBounds.max = itemMax;
                firstBound = false;
            } else {
                totalBounds.min = glm::min(totalBounds.min, itemMin);
                totalBounds.max = glm::max(totalBounds.max, itemMax);
            }
        }

        bounds = totalBounds;
        isLeaf = itemCount <= 4u;

        if (!isLeaf) {
            const glm::vec3 extent = totalBounds.max - totalBounds.min;
            const int axis = extent.y > extent.x
                ? (extent.z > extent.y ? 2 : 1)
                : (extent.z > extent.x ? 2 : 0);
            const size_t middle = first + itemCount / 2u;
            std::nth_element(
                itemsList.begin() + static_cast<std::ptrdiff_t>(first),
                itemsList.begin() + static_cast<std::ptrdiff_t>(middle),
                itemsList.begin() + static_cast<std::ptrdiff_t>(last),
                [axis](const SpatialItem& a, const SpatialItem& b) {
                    return a.position[axis] < b.position[axis];
                }
            );

            BVHNode& leftNode = pool.acquire();
            BVHNode& rightNode = pool.acquire();
            left = &leftNode;
            right = &rightNode;
            left->build(itemsList, first, middle, pool);
            right->build(itemsList, middle, last, pool);
        }
    }

    void BVHNode::query(const AABB& range, std::vector<SpatialItem>& result) const {
        if (!bounds.intersects(range)) {
            return;
        }

        if (isLeaf) {
            for (const auto& item : getItems()) {
                const glm::vec3 radius{item.radius};
                if (AABB{item.position - radius, item.position + radius}.intersects(range)) {
                    result.push_back(item);
                }
            }
            return;
        }

        if (left) left->query(range, result);
        if (right) right->query(range, result);
    }

    void BVHNode::getCollisions(std::vector<CollisionInfo>& collisions) const {
        if (isLeaf) {
            // Check pairwise collisions within this leaf
            const auto leafItems = getItems();
            for (size_t i = 0; i < leafItems.size(); ++i) {
                for (size_t j = i + 1; j < leafItems.size(); ++j) {
                    testPair(leafItems[i], leafItems[j], collisions);
                }
            }
            return;
        }

        // Recursively check children, then check across the split boundary —
        // otherwise two items on opposite sides of the split that still overlap
        // in space would never be tested against each other.
        if (left) left->getCollisions(collisions);
        if (right) right->getCollisions(collisions);
        if (left && right) crossCheck(*left, *right, collisions);
    }

    // BVH Implementation
    BVH::BVH() = default;

    void BVH::build(const std::vector<SpatialItem>& items) {
        itemCount = items.size();
        workingItems.assign(items.begin(), items.end());
        nodePool.reset();
        root = &nodePool.acquire();
        root->build(workingItems, 0u, workingItems.size(), nodePool);
    }

    void BVH::query(const AABB& range, std::vector<SpatialItem>& result) const {
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
