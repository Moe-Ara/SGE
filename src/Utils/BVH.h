#ifndef GLCPP_BVH_H
#define GLCPP_BVH_H

#include <memory>
#include <vector>
#include "../Physics/Collider.h"
#include "../GameObjects/GameObject.h"
#include "../Physics/BoundingVolume.h"

namespace SGE::UTILS {

    class BVH {
    public:
        BVH();
        ~BVH();

        void build(const std::vector<std::shared_ptr<SGE::GAMEOBJECTS::GameObject>>& objects);
        void insert(std::shared_ptr<SGE::GAMEOBJECTS::GameObject> object);
        std::vector<std::shared_ptr<SGE::GAMEOBJECTS::GameObject>> query(const std::shared_ptr<SGE::PHYSICS::BoundingVolume>& queryVolume);

        struct BVHNode {
            std::shared_ptr<SGE::PHYSICS::BoundingVolume> boundingVolume;
            std::unique_ptr<BVHNode> left;
            std::unique_ptr<BVHNode> right;
            std::vector<std::shared_ptr<SGE::GAMEOBJECTS::GameObject>> objects;
        };

    private:
        std::unique_ptr<BVHNode> root;

        void buildRecursively(std::unique_ptr<BVHNode>& node, const std::vector<std::shared_ptr<SGE::GAMEOBJECTS::GameObject>>& objects, int depth);
        void insertRecursively(std::unique_ptr<BVHNode>& node, std::shared_ptr<SGE::GAMEOBJECTS::GameObject> object, int depth);
        void queryRecursively(const std::unique_ptr<BVHNode>& node, const std::shared_ptr<SGE::PHYSICS::BoundingVolume>& queryVolume, std::vector<std::shared_ptr<SGE::GAMEOBJECTS::GameObject>>& results);

        bool shouldInsertLeft(const std::unique_ptr<BVHNode>& left, const std::shared_ptr<SGE::GAMEOBJECTS::GameObject>& object);

        void splitObjects(const std::vector<std::shared_ptr<SGE::GAMEOBJECTS::GameObject>>& objects,
                          std::vector<std::shared_ptr<SGE::GAMEOBJECTS::GameObject>>& leftObjects,
                          std::vector<std::shared_ptr<SGE::GAMEOBJECTS::GameObject>>& rightObjects);

        std::shared_ptr<SGE::PHYSICS::BoundingVolume>
        computeBoundingVolume(const std::vector<std::shared_ptr<SGE::GAMEOBJECTS::GameObject>>& objects);
    };

} // namespace SGE::UTILS

#endif // GLCPP_BVH_H
