//#include "BVH.h"
//#include "../Physics/BoundingVolumes/SphereBoundingVolume.h"
//#include "../Physics/BoundingVolumes/CubeBoundingVolume.h"
//
//namespace SGE {
//    namespace UTILS {
//
//        BVH::BVH() : root(nullptr) {}
//
//        BVH::~BVH() = default;
//
//        void BVH::build(const std::vector<std::shared_ptr<SGE::GAMEOBJECTS::GameObject>>& objects) {
//            if (objects.empty()) return;
//            root = std::make_unique<BVHNode>();
//            buildRecursively(root, objects, 0);
//        }
//
//        void BVH::insert(std::shared_ptr<SGE::GAMEOBJECTS::GameObject> object) {
//            if (!root) {
//                root = std::make_unique<BVHNode>();
//            }
//            insertRecursively(root, std::move(object), 0);
//        }
//
//        std::vector<std::shared_ptr<SGE::GAMEOBJECTS::GameObject>> BVH::query(const std::shared_ptr<SGE::PHYSICS::BoundingVolume>& queryVolume) {
//            std::vector<std::shared_ptr<SGE::GAMEOBJECTS::GameObject>> results;
//            if (root) {
//                queryRecursively(root, queryVolume, results);
//            }
//            return results;
//        }
//
//        void BVH::buildRecursively(std::unique_ptr<BVHNode>& node, const std::vector<std::shared_ptr<SGE::GAMEOBJECTS::GameObject>>& objects, int depth) {
//            node->boundingVolume = computeBoundingVolume(objects);
//            if (objects.size() <= 1) {
//                node->objects = objects;
//                return;
//            }
//
//            std::vector<std::shared_ptr<SGE::GAMEOBJECTS::GameObject>> leftObjects;
//            std::vector<std::shared_ptr<SGE::GAMEOBJECTS::GameObject>> rightObjects;
//            splitObjects(objects, leftObjects, rightObjects);
//
//            node->left = std::make_unique<BVHNode>();
//            node->right = std::make_unique<BVHNode>();
//            buildRecursively(node->left, leftObjects, depth + 1);
//            buildRecursively(node->right, rightObjects, depth + 1);
//        }
//
//        void BVH::insertRecursively(std::unique_ptr<BVHNode>& node, std::shared_ptr<SGE::GAMEOBJECTS::GameObject> object, int depth) {
//            if (!node->boundingVolume) {
//                node->boundingVolume = object->getCollider()->getOBB();
//                node->objects.push_back(object);
//                return;
//            }
//
//            node->boundingVolume = computeBoundingVolume(node->objects);
//
//            if (shouldInsertLeft(node->left, object)) {
//                if (!node->left) node->left = std::make_unique<BVHNode>();
//                insertRecursively(node->left, std::move(object), depth + 1);
//            } else {
//                if (!node->right) node->right = std::make_unique<BVHNode>();
//                insertRecursively(node->right, std::move(object), depth + 1);
//            }
//        }
//
//        void BVH::queryRecursively(const std::unique_ptr<BVHNode>& node, const std::shared_ptr<SGE::PHYSICS::BoundingVolume>& queryVolume, std::vector<std::shared_ptr<SGE::GAMEOBJECTS::GameObject>>& results) {
//            if (!node->boundingVolume || !node->boundingVolume->intersects(*queryVolume)) {
//                return;
//            }
//
//            if (!node->left && !node->right) {
//                for (const auto& obj : node->objects) {
//                    if (obj->getCollider()->checkCollision(*queryVolume)) {
//                        results.push_back(obj);
//                    }
//                }
//            } else {
//                if (node->left) queryRecursively(node->left, queryVolume, results);
//                if (node->right) queryRecursively(node->right, queryVolume, results);
//            }
//        }
//
//        bool BVH::shouldInsertLeft(const std::unique_ptr<BVH::BVHNode>& left, const std::shared_ptr<SGE::GAMEOBJECTS::GameObject>& object) {
//            // Example heuristic: insert based on position
//            return object->getTransform().getMTranslation().x < left->boundingVolume->getCenter().x;
//        }
//
//        void BVH::splitObjects(const std::vector<std::shared_ptr<SGE::GAMEOBJECTS::GameObject>>& objects,
//                               std::vector<std::shared_ptr<SGE::GAMEOBJECTS::GameObject>>& leftObjects,
//                               std::vector<std::shared_ptr<SGE::GAMEOBJECTS::GameObject>>& rightObjects) {
//            // Simple median split on the x-axis
//            size_t mid = objects.size() / 2;
//            std::nth_element(objects.begin(), objects.begin() + mid, objects.end(),
//                             [](const auto& a, const auto& b) {
//                                 return a->getTransform().translation.x < b->getTransform().translation.x;
//                             });
//            leftObjects.assign(objects.begin(), objects.begin() + mid);
//            rightObjects.assign(objects.begin() + mid, objects.end());
//        }
//
//        std::shared_ptr<SGE::PHYSICS::BoundingVolume> BVH::computeBoundingVolume(const std::vector<std::shared_ptr<SGE::GAMEOBJECTS::GameObject>>& objects) {
//            // Compute a bounding volume that encompasses all objects
//            // Placeholder implementation: use a cube that bounds all objects
//            glm::vec3 min(FLT_MAX);
//            glm::vec3 max(-FLT_MAX);
//
//            for (const auto& object : objects) {
//                auto collider = object->getCollider();
//                if (!collider) continue;
//                auto boundingVolume = collider->getOBB();
//                auto box = dynamic_cast<CubeBoundingVolume*>(boundingVolume.get());
//                if (box) {
//                    min = glm::min(min, box->getMin());
//                    max = glm::max(max, box->getMax());
//                }
//            }
//
//            return std::make_shared<CubeBoundingVolume>(min, max);
//        }
//
//    } // namespace UTILS
//} // namespace SGE
