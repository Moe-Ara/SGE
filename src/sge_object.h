#ifndef GLCPP_SGE_OBJECT_H
#define GLCPP_SGE_OBJECT_H

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <memory>
#include "sge_model.h"

namespace SGE::actors {

    struct Transform {
        glm::vec3 translation{};
        glm::vec3 scale{1.f, 1.f, 1.f};
        glm::quat rotation{};

        glm::mat4 mat4() const;
        glm::mat3 normalMatrix() const;
    };

    class sge_object {
    public:
        using id_t = unsigned int;

        static sge_object CreateGameObject() {
            static id_t currentId = 0;
            return sge_object(currentId++);
        }

        sge_object(const sge_object &) = delete;
        sge_object &operator=(const sge_object &) = delete;
        sge_object(sge_object &&) = default;
        sge_object &operator=(sge_object &&) = default;

        id_t getId() const { return id; }

        std::shared_ptr<sge_model> model{};
        glm::vec3 color{1.f, 1.f, 1.f}; // Default color to white
        Transform transform{};

    private:
        explicit sge_object(id_t objectId) : id(objectId) {}
        id_t id;
    };

}

#endif //GLCPP_SGE_OBJECT_H
