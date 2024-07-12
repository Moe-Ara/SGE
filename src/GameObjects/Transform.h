//
// Created by Mohamad on 11/07/2024.
//

#ifndef GLCPP_TRANSFORM_H
#define GLCPP_TRANSFORM_H

#include <glm/vec3.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/quaternion_float.hpp>

namespace SGE::GAMEOBJECTS{
    class Transform {
    public:
        const glm::vec3 &getMTranslation() const;

        const glm::vec3 &getMScale() const;

        const glm::quat &getMRotation() const;

    private:
        glm::vec3 m_translation{};
        glm::vec3 m_scale{1.f, 1.f, 1.f};
        glm::quat m_rotation{};


    public:
        void translate(const glm::vec3& translation);
        void scale(const glm::vec3& scale);
        void rotate(const glm::quat& rotation);
        [[nodiscard]] glm::mat4 mat4() const;
        [[nodiscard]] glm::mat3 normalMatrix() const;
    };
}



#endif //GLCPP_TRANSFORM_H
