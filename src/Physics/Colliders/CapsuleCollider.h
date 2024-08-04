//
// Created by Mohamad on 13/07/2024.
//

#ifndef GLCPP_CAPSULECOLLIDER_H
#define GLCPP_CAPSULECOLLIDER_H

#include <memory>
#include "../Collider.h"
#include "../../GameObjects/Model.h"
#include "../../Utils/ModelLoader.h"
#include <cfloat>
#include <algorithm>
namespace SGE::PHYSICS {

    class CapsuleCollider : public Collider {
    public:
        CapsuleCollider();

        ~CapsuleCollider() override = default;

        bool checkCollision(SGE::PHYSICS::Collider &other) override;

        void update(const GAMEOBJECTS::Transform &position) override;

        void render() const override;

        const ColliderType getType() override;

        [[nodiscard]] GAMEOBJECTS::Transform getTransform() const override;

        [[nodiscard]] std::shared_ptr<SGE::PHYSICS::BoundingVolume> getOBB() const override;

        float getMThreshold() const;

        void setMThreshold(float mThreshold);

        float getMRadius() const;

        float getMHeight() const;
        static void projectCapsuleOnAxis(const glm::vec3 &capsuleStart, const glm::vec3 &capsuleEnd, float capsuleRadius,
                                         const glm::vec3 &axis, float &min, float &max);
    private:
        SGE::GAMEOBJECTS::Transform m_transform{};
        std::shared_ptr<SGE::GAMEOBJECTS::Model> m_model{SGE::UTILS::ModelLoader::loadModelFromFile("capsule.obj")};
        float m_radius{.4f};
        float m_height{1.2f};

        [[nodiscard]] float calculateRadius() const;

        float m_threshold{0.05f};


    };
}


#endif //GLCPP_CAPSULECOLLIDER_H
