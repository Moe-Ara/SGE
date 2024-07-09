#ifndef GLCPP_GAMEOBJECT_H
#define GLCPP_GAMEOBJECT_H

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <memory>
#include "Model.h"

namespace SGE::actors {

    struct Transform {
        glm::vec3 translation{};
        glm::vec3 scale{1.f, 1.f, 1.f};
        glm::quat rotation{};

        [[nodiscard]] glm::mat4 mat4() const;
        [[nodiscard]] glm::mat3 normalMatrix() const;
    };

    class GameObject {
    public:
        using id_t = unsigned int;

        static GameObject CreateGameObject() {
            static id_t currentId = 0;
            return GameObject(currentId++);
        }

        GameObject(const GameObject &) = delete;
        GameObject &operator=(const GameObject &) = delete;
        GameObject(GameObject &&) = default;
        GameObject &operator=(GameObject &&) = default;

        id_t getId() const { return id; }

        std::shared_ptr<Model> model{};
        glm::vec3 color{1.f, 1.f, 1.f}; // Default color to white
        Transform transform{};

    private:
        explicit GameObject(id_t objectId) : id(objectId) {}
        id_t id;
    };

}

#endif //GLCPP_GAMEOBJECT_H
