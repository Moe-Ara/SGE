//
// Created by Mohamad on 07/07/2024.
//

#ifndef GLCPP_NPC_H
#define GLCPP_NPC_H

#include "../GameObjects/Actor.h"
#include "../Utils/ModelLoader.h"
#include "../Physics/Colliders/CubeCollider.h"
#include "../Physics/Colliders/OBBCollider.h"

namespace SGE::DEBUGGING {
    class npc : public GAMEOBJECTS::Actor {
    public:

        void move(glm::vec3 movement, float deltaTime) override;

        ~npc() override;

        explicit npc(long id);

        [[nodiscard]] glm::vec3 getColor() const override;
        std::shared_ptr<SGE::PHYSICS::Collider> getCollider() override;

    private:
        std::shared_ptr<SGE::GAMEOBJECTS::Model> modelPtr;
        std::shared_ptr<SGE::PHYSICS::SphereCollider> m_collider;


    };
};


#endif //GLCPP_NPC_H
