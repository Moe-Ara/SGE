//
// Created by Mohamad on 07/07/2024.
//

#ifndef GLCPP_NPC_H
#define GLCPP_NPC_H

#include "../GameObjects/Actor.h"
#include "../Utils/ModelLoader.h"

namespace SGE::DEBUGGING {
    class npc : public SGE::GAMEOBJECTS::Actor {
    public:
        explicit npc(long id);
        ~npc() override;

        void move(glm::vec3 movement, float deltaTime) override;
        [[nodiscard]] glm::vec3 getColor() const override;
        void render(const std::shared_ptr<SGE::GRAPHICS::Shader>& shader) override;

    private:
        std::shared_ptr<SGE::GAMEOBJECTS::Model> modelPtr;
    };
};


#endif //GLCPP_NPC_H
