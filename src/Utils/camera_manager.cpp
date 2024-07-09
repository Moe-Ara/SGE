//
// Created by Mohamad on 12/06/2024.
//

#include "camera_manager.h"

glm::vec3 SGE::graphics::camera_manager::updateKeyboard(const SGE::Input::input_handler &inputHandler) {

}

glm::vec3 SGE::graphics::camera_manager::updateMouse(const SGE::Input::input_handler &inputHandler) {


}

const SGE::graphics::Camera &SGE::graphics::camera_manager::getCamera() {
    return camera;
}

void SGE::graphics::camera_manager::update(const SGE::Input::input_handler &inputHandler) {
}

SGE::graphics::camera_manager::camera_manager(SGE::graphics::Camera &camera) : camera(camera){

}

