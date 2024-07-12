//
// Created by Mohamad on 11/07/2024.
//

#ifndef GLCPP_COORDINATESYSTEM_H
#define GLCPP_COORDINATESYSTEM_H

#include "../Graphics/Shader.h"

namespace SGE::DEBUGGING {
    class CoordinateSystem {
    public:
        CoordinateSystem(SGE::GRAPHICS::Shader &shader);

        ~CoordinateSystem();

        void render(const glm::mat4 &view, const glm::mat4 &projection);

    private:
        GLuint axisVAO, axisVBO;
        SGE::GRAPHICS::Shader &shader;

        void setupAxisBuffers();
    };

}


#endif //GLCPP_COORDINATESYSTEM_H
