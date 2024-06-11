//
// Created by Mohamad on 12/06/2024.
//

#ifndef GLCPP_SGE_CAMERA_MANAGER_H
#define GLCPP_SGE_CAMERA_MANAGER_H

#include <glm/vec3.hpp>
#include "sge_camera.h"
#include "sge_input_handler.h"

namespace SGE::graphics{
    class sge_camera_manager {
    public:
        explicit sge_camera_manager(SGE::graphics::sge_camera& camera);
        void update(const SGE::utils::sge_input_handler& inputHandler);
        const SGE::graphics::sge_camera& getCamera();
    private:
        SGE::graphics::sge_camera& camera;
        bool isMousePressed {};
        double lastMouseX{};
        double lastMouseY{};
        glm::vec3 cameraPosition{0.0f, 0.0f, 10.0f};
        glm::vec3 direction{0.0f,0.0f,0.0f};
        glm::vec3 updateMouse(const SGE::utils::sge_input_handler& inputHandler);
        glm::vec3 updateKeyboard(const SGE::utils::sge_input_handler& inputHandler);
    };
}



#endif //GLCPP_SGE_CAMERA_MANAGER_H
