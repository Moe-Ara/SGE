//
// Created by Mohamad on 12/06/2024.
//

#ifndef GLCPP_CAMERA_MANAGER_H
#define GLCPP_CAMERA_MANAGER_H

#include <glm/vec3.hpp>
#include "../Graphics/Camera.h"
#include "../Input/input_handler.h"

namespace SGE::graphics{
    class camera_manager {
    public:
        explicit camera_manager(SGE::graphics::Camera& camera);
        void update(const SGE::Input::input_handler& inputHandler);
        const SGE::graphics::Camera& getCamera();
    private:
        SGE::graphics::Camera& camera;
        bool isMousePressed {};
        double lastMouseX{};
        double lastMouseY{};
        glm::vec3 cameraPosition{0.0f, 0.0f, 10.0f};
        glm::vec3 direction{0.0f,0.0f,0.0f};
        glm::vec3 updateMouse(const SGE::Input::input_handler& inputHandler);
        glm::vec3 updateKeyboard(const SGE::Input::input_handler& inputHandler);
    };
}



#endif //GLCPP_CAMERA_MANAGER_H
