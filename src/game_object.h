//
// Created by Mohamad on 17/04/2024.
//

#ifndef GLCPP_GAME_OBJECT_H
#define GLCPP_GAME_OBJECT_H

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <memory>
#include "game_model.h"


namespace glcpp {
    struct Transform {
        glm::vec3 translation{};
        glm::vec3 scale{1.f, 1.f, 1.f};
        glm::quat rotation{};

        glm::mat4 mat4();

        glm::mat3 normalMatrix();
    };

    class game_object {
    public:
        using id_t = unsigned int;

        static game_object CreateGameObject() {
            static id_t currentId = 0;
            return game_object(currentId++);
        }

        game_object(const game_object &) = delete;

        game_object &operator=(const game_object &) = delete;

        game_object(game_object &&) = default;

        game_object &operator=(game_object &&) = default;

        id_t getId() { return id; }

        std::shared_ptr<game_model> model{};
        glm::vec3 color{};
        Transform transform{};
    private:
        explicit game_object(id_t objectId) : id(objectId) {}

        id_t id;

    };
}


#endif //GLCPP_GAME_OBJECT_H
