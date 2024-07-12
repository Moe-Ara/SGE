//
// Created by Mohamad on 12/07/2024.
//

#ifndef GLCPP_RENDERER_H
#define GLCPP_RENDERER_H

#include <memory>
#include "Shader.h"
#include "Camera.h"
#include "Scene.h"

namespace SGE::GRAPHICS{
    class Renderer {
    public:
        Renderer(std::shared_ptr<SGE::GRAPHICS::Shader> shader, std::shared_ptr<SGE::GRAPHICS::Camera> camera);
        void render(const Scene& scene);

    private:
        std::shared_ptr<SGE::GRAPHICS::Shader> shader;
        std::shared_ptr<SGE::GRAPHICS::Camera> camera;

        void renderObject(const std::shared_ptr<SGE::GAMEOBJECTS::GameObject>& object);
    };
}



#endif //GLCPP_RENDERER_H
