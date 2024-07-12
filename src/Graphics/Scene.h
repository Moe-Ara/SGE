//
// Created by Mohamad on 11/07/2024.
//

#ifndef GLCPP_SCENE_H
#define GLCPP_SCENE_H
#include <vector>
#include <memory>
#include "../GameObjects/GameObject.h"
namespace SGE::GRAPHICS{
    class Scene {
    public:
        Scene();
        ~Scene();
        void update(float deltaTime);
        void addObject(std::shared_ptr<SGE::GAMEOBJECTS::GameObject> object);
        [[nodiscard]] const std::vector<std::shared_ptr<SGE::GAMEOBJECTS::GameObject>>& getGameObjects() const;
    private:
        std::vector<std::shared_ptr<SGE::GAMEOBJECTS::GameObject>> gameObjects;
    };
}



#endif //GLCPP_SCENE_H
