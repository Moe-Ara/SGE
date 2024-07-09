#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "src/GameObjects/GameObject.h"
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "src/GameObjects/Model.h"
#include "src/Graphics/Window.h"
#include "src/Input/input_handler.h"
#include "src/Graphics/Shader.h"
#include "src/Graphics/Camera.h"
#include "src/Utils/camera_manager.h"
#include "src/GameObjects/Player.h"
#include "src/GameObjects/npc.h"
#include "src/Utils/Material.h"
#include "src/Utils/Light.h"
#include <chrono>

void errorCallback(int error, const char *description) {
    std::cerr << "Error: " << description << std::endl;
}

void renderObjects(SGE::graphics::Shader& shader, SGE::actors::Actor& actor, SGE::graphics::Camera& camera) {
    // Set light properties
    SGE::utils::Light light;
    light.position = glm::vec3(10.0f, 5.0f, 0.0f);
    light.ambient = glm::vec3(0.2f, 0.2f, 0.2f);
    light.diffuse = glm::vec3(0.5f, 0.5f, 0.5f);
    light.specular = glm::vec3(1.0f, 1.0f, 1.0f);

    // Set material properties
    SGE::utils::Material material;
    material.ambient = glm::vec3(1.0f, 0.5f, 0.31f);
    material.diffuse = glm::vec3(1.0f, 0.5f, 0.31f);
    material.specular = glm::vec3(0.5f, 0.5f, 0.5f);
    material.shininess = 32.0f;

    shader.enable();
    shader.setUniformMat4("transform", actor.getTransform().mat4());
    shader.setUniformMat4("projection", camera.getProjection());
    shader.setUniformMat4("view", camera.getView());
    shader.setUniformMat3("normalMatrix", actor.getTransform().normalMatrix());

    shader.setUniformFloat3("uniformColor", actor.getColor());
    shader.setUniformFloat3("material.ambient", material.ambient);
    shader.setUniformFloat3("material.diffuse", material.diffuse);
    shader.setUniformFloat3("material.specular", material.specular);
    shader.setUniformFloat1("material.shininess", material.shininess);

    shader.setUniformFloat3("light.position", light.position);
    shader.setUniformFloat3("light.ambient", light.ambient);
    shader.setUniformFloat3("light.diffuse", light.diffuse);
    shader.setUniformFloat3("light.specular", light.specular);

    // Set view position (camera position)
    shader.setUniformFloat3("viewPos", camera.getPositionFromViewMatrix());

    actor.render();
}

int main() {
    // Set GLFW error callback
    glfwSetErrorCallback(errorCallback);

    // Initialize window
    SGE::graphics::Window window("SGE", 960, 540);
    SGE::Input::input_handler::setupKeyHandler(&window);

    SGE::Input::input_handler inputHandler({GLFW_KEY_A, GLFW_KEY_S, GLFW_KEY_W, GLFW_KEY_D, GLFW_KEY_LEFT_CONTROL, GLFW_KEY_LEFT_SHIFT, GLFW_KEY_Q, GLFW_KEY_E},
                                           {GLFW_MOUSE_BUTTON_RIGHT, GLFW_MOUSE_BUTTON_LEFT});

    glClearColor(0.f, 0.2f, 0.3f, 1.0f);

    auto model = SGE::actors::Model::createModelFromFile("cube.obj");

    SGE::actors::Player player{};
    SGE::graphics::Shader shader("vertex.vert", "fragment.frag");
    SGE::actors::npc NpC{};

    auto lastFrameTime = std::chrono::high_resolution_clock::now();

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Disable blending if it's not required for opaque objects
    glDisable(GL_BLEND);

    while (!window.closed()) {
        auto currentFrameTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> deltaTimeDuration = currentFrameTime - lastFrameTime;
        float deltaTime = deltaTimeDuration.count();

        // Update player and other game objects
        player.update(deltaTime);

        // Clear the screen
        window.clear();
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Ensure we're filling polygons, not just drawing lines

        // Render objects
        renderObjects(shader, player, player.getCamera());
        renderObjects(shader, NpC, player.getCamera());

        // Update the window (swap buffers, poll events)
        window.update();
        lastFrameTime = currentFrameTime;
    }

    return 0;
}
