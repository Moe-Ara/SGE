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
                info.contactPoint = (itemA.position + itemB.position) * 0.5f;
                info.normal = distance > 0.0f
                    ? (itemB.position - itemA.position) / distance
                    : glm::vec3(0.0f, 1.0f, 0.0f);
                info.penetrationDepth = (itemA.radius + itemB.radius) - distance;
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

    void BVHNode::build(const std::vector<SpatialItem>& itemsList) {
        items = itemsList;
        left.reset();
        right.reset();

        if (items.empty()) {
            isLeaf = true;
            return;
        }

        // Calculate bounding box for all items
        AABB totalBounds;
        bool first = true;

        for (const auto& item : items) {
            if (first) {
                totalBounds.min = item.position;
                totalBounds.max = item.position;
                first = false;
            } else {
                totalBounds.min = glm::min(totalBounds.min, item.position);
                totalBounds.max = glm::max(totalBounds.max, item.position);
            }
        }

        bounds = totalBounds;
        isLeaf = items.size() <= 4; // Simple leaf threshold

        if (!isLeaf) {
            // Simple splitting - divide by x-axis for now
            float split = (totalBounds.min.x + totalBounds.max.x) * 0.5f;

            std::vector<SpatialItem> leftItems, rightItems;

            for (const auto& item : items) {
                if (item.position.x <= split) {
                    leftItems.push_back(item);
                } else {
                    rightItems.push_back(item);
                }
            }

            // If the split didn't separate anything (e.g. all items share the same
            // x position), treat this node as a leaf instead of recursing forever.
            if (leftItems.empty() || rightItems.empty()) {
                isLeaf = true;
                return;
            }

            left = std::make_unique<BVHNode>();
            left->build(leftItems);

            right = std::make_unique<BVHNode>();
            right->build(rightItems);
        }
    }

    void BVHNode::query(const AABB& range, std::vector<SpatialItem>& result) const {
        if (!bounds.intersects(range)) {
            return;
        }

        if (isLeaf) {
            for (const auto& item : items) {
                result.push_back(item);
            }
            return;
        }

        if (left) left->query(range, result);
        if (right) right->query(range, result);
    }

    void BVHNode::getCollisions(std::vector<CollisionInfo>& collisions) const {
        if (isLeaf) {
            // Check pairwise collisions within this leaf
            for (size_t i = 0; i < items.size(); ++i) {
                for (size_t j = i + 1; j < items.size(); ++j) {
                    testPair(items[i], items[j], collisions);
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
    BVH::BVH() {
        root = std::make_unique<BVHNode>();
    }

    void BVH::build(const std::vector<SpatialItem>& items) {
        itemCount = items.size();
        root = std::make_unique<BVHNode>();
        root->build(items);
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
