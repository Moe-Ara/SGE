//
// Created by Mohamad on 13/07/2024.
//

#ifndef GLCPP_PLANECOLLIDER_H
#define GLCPP_PLANECOLLIDER_H

#include "../Collider.h"

namespace SGE::PHYSICS {
    class PlaneCollider : public Collider {
    public:
        ~PlaneCollider() override = default;

        bool checkCollision(Collider &other) override;

        void update(const GAMEOBJECTS::Transform &position) override;

        void render() const override;

        const ColliderType getType() override;

        GAMEOBJECTS::Transform getTransform() const override;

        std::shared_ptr<BoundingVolume> getOBB() const override;

        const glm::vec3 &getMNormal() const;

        float m_distance{};

        float getMDistance() const;


    private:
        SGE::GAMEOBJECTS::Transform m_transform;
        glm::vec3 m_normal;


    };
}


#endif //GLCPP_PLANECOLLIDER_H
