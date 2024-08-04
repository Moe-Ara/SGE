#ifndef GLCPP_SPHERECOLLIDER_H
#define GLCPP_SPHERECOLLIDER_H

#include "../Collider.h"
#include "../../Utils/ModelLoader.h"

#include "../BoundingVolumes/OrientedBoundingBoxes.h"


namespace SGE::PHYSICS {


    class SphereCollider : public Collider {
    public:
        SphereCollider();
        ~SphereCollider() override = default;

        bool checkCollision(Collider& other) override;
        void update(const SGE::GAMEOBJECTS::Transform &position) override;
        void render() const override;
        std::shared_ptr<SGE::PHYSICS::BoundingVolume> getOBB() const override;
        const ColliderType getType() override;
        [[nodiscard]] GAMEOBJECTS::Transform getTransform() const override;
        float getMThreshold() const;
        void setMThreshold(float mThreshold);
        float getMRadius() const;

    private:
        SGE::GAMEOBJECTS::Transform m_transform{};
        std::shared_ptr<SGE::GAMEOBJECTS::Model> m_model{SGE::UTILS::ModelLoader::loadModelFromFile("sphere.obj")};
        float m_radius{};
        [[nodiscard]] float calculateRadius() const;
        float m_threshold{0.05f};
    };
}

#endif //GLCPP_SPHERECOLLIDER_H
