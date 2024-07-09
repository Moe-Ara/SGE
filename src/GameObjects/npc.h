//
// Created by Mohamad on 07/07/2024.
//

#ifndef GLCPP_NPC_H
#define GLCPP_NPC_H

#include "Actor.h"

namespace SGE::actors{
    class npc: public Actor{
    public:
        void jump(float deltaTime) override;

        void move(glm::vec3 movement, float deltaTime) override;

        void fire(float deltaTime) override;

        void dash(float deltaTime) override;

        ~npc() override;

        Transform getTransform() override;

        void update(float deltaTime) override;

        void render() override;
        npc();
        glm::vec3 getColor() override;
    private:
        std::shared_ptr<SGE::actors::Model> modelPtr;
        Transform transform{};
    };
};



#endif //GLCPP_NPC_H
