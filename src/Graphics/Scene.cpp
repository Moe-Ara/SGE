//
// Created by Mohamad on 11/07/2024.
//

#include "Scene.h"

SGE::GRAPHICS::Scene::Scene(){
    gameObjects = std::vector<std::shared_ptr<SGE::GAMEOBJECTS::GameObject>>();
}

SGE::GRAPHICS::Scene::~Scene() {

}

void SGE::GRAPHICS::Scene::update(float deltaTime) {
    for (auto& gameObject : gameObjects) {
        gameObject->update(deltaTime);
    }
}

void SGE::GRAPHICS::Scene::addObject(std::shared_ptr<SGE::GAMEOBJECTS::GameObject> object) {
    gameObjects.push_back(object);
}

const std::vector<std::shared_ptr<SGE::GAMEOBJECTS::GameObject>> &SGE::GRAPHICS::Scene::getGameObjects() const {
    return gameObjects;
}


