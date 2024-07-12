//
// Created by Mohamad on 11/07/2024.
//

#include "GameObject.h"

SGE::GAMEOBJECTS::GameObject::GameObject(Transform transform,
                                         std::shared_ptr<Model> model): transform(transform),model(model) {

}

SGE::GAMEOBJECTS::GameObject::~GameObject() {

}

SGE::GAMEOBJECTS::Transform &SGE::GAMEOBJECTS::GameObject::getTransform() {
    return transform;;
}

void SGE::GAMEOBJECTS::GameObject::update(float deltaTime) {

}

void SGE::GAMEOBJECTS::GameObject::render() {
    if (model) {
        model->bind();
        model->render();
        model->unbind();
    }
}
