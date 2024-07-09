//
// Created by Mohamad on 07/07/2024.
//

#include "npc.h"

void SGE::actors::npc::jump(float deltaTime) {

}

void SGE::actors::npc::move(glm::vec3 movement, float deltaTime) {

}

void SGE::actors::npc::fire(float deltaTime) {

}

void SGE::actors::npc::dash(float deltaTime) {

}

SGE::actors::npc::~npc() {

}

SGE::actors::Transform SGE::actors::npc::getTransform() {
    return Transform();
}

void SGE::actors::npc::update(float deltaTime) {

}

void SGE::actors::npc::render() {

    modelPtr->bind();
    modelPtr->render();
    modelPtr->unbind();
}

SGE::actors::npc::npc() {
    modelPtr = SGE::actors::Model::createModelFromFile("cube.obj");
    transform.rotation=glm::vec3 (0.5f,1.f,1.f);
}

glm::vec3 SGE::actors::npc::getColor() {
    return {1.0f, 1.0f, 1.0f};
}
