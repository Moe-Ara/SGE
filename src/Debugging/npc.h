//
// Created by Mohamad on 07/07/2024.
//

#ifndef GLCPP_NPC_H
#define GLCPP_NPC_H

#include "../GameObjects/Actor.h"
#include "../Utils/ModelLoader.h"

namespace SGE::DEBUGGING {
    class npc : public GAMEOBJECTS::Actor {
    public:

        void move(glm::vec3 movement, float deltaTime) override;

        ~npc() override;

        explicit npc(long id);

        [[nodiscard]] glm::vec3 getColor() const override;

    private:
        std::shared_ptr<SGE::GAMEOBJECTS::Model> modelPtr;
    };
};


#endif //GLCPP_NPC_H
