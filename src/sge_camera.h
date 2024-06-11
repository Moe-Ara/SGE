//
// Created by Mohamad on 29/04/2024.
//

#ifndef GLCPP_SGE_CAMERA_H
#define GLCPP_SGE_CAMERA_H
#include <memory>
#include <glm/glm.hpp>
namespace SGE::graphics {
    class sge_camera {
    public:

        void setOrthographicProjection(float left, float right, float top, float bottom, float near, float far);
        void setPrespectiveProjection(float fovy, float aspect, float near, float far);

        void setViewDirection(glm::vec3 position, glm::vec3 direction, glm::vec3 up= glm::vec3(0.f, -1.f, 0.f));
        void setViewTarget(glm::vec3 position, glm::vec3 target, glm::vec3 up= glm::vec3(0.f, -1.f, 0.f));
        void setViewYXZ(glm::vec3 position, glm::vec3 rotation);


        [[nodiscard]] const glm::mat4& getProjection() const;
        [[nodiscard]] const glm::mat4& getView()const;

    private:
        glm::mat4 projectionMatrix{1.f};
        glm::mat4 viewMatrix{1.f};
    public:
        [[nodiscard]] glm::vec3 getRight() const;

        [[nodiscard]] glm::vec3 getRelativeUp() const;
        [[nodiscard]] glm::vec3 getAbsoluteUp() const;

        [[nodiscard]] glm::vec3 getForward() const;
        glm::vec3 getPositionFromViewMatrix();
        glm::mat3 getRotationFromViewMatrix();

    };
}

#endif //GLCPP_SGE_CAMERA_H
