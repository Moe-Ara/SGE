//
// Created by Mohamad on 28/07/2024.
//

#ifndef GLCPP_CAPSULENPC_H
#define GLCPP_CAPSULENPC_H

#include "../GameObjects/Actor.h"
#include "../Physics/Colliders/CapsuleCollider.h"

namespace SGE {
    namespace DEBUGGING {

    class capsulenpc : public GAMEOBJECTS::Actor{
    public:

        void move(glm::vec3 movement, float deltaTime) override;

        ~capsulenpc() override;

        explicit capsulenpc(long id);

        [[nodiscard]] glm::vec3 getColor() const override;
        std::shared_ptr<SGE::PHYSICS::Collider> getCollider() override;

    private:
        std::shared_ptr<SGE::GAMEOBJECTS::Model> modelPtr;
        std::shared_ptr<SGE::PHYSICS::CapsuleCollider> m_collider;
        };

    } // DEBUGGING
} // SGE

#endif //GLCPP_CAPSULENPC_H
