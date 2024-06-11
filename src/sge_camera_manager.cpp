//
// Created by Mohamad on 12/06/2024.
//

#include "sge_camera_manager.h"

glm::vec3 SGE::graphics::sge_camera_manager::updateKeyboard(const SGE::utils::sge_input_handler &inputHandler) {

}

glm::vec3 SGE::graphics::sge_camera_manager::updateMouse(const SGE::utils::sge_input_handler &inputHandler) {


}

const SGE::graphics::sge_camera &SGE::graphics::sge_camera_manager::getCamera() {
    return camera;
}

void SGE::graphics::sge_camera_manager::update(const SGE::utils::sge_input_handler &inputHandler) {
}

SGE::graphics::sge_camera_manager::sge_camera_manager(SGE::graphics::sge_camera &camera) :camera(camera){

}

