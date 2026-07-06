#ifndef RENDERER_H
#define RENDERER_H

#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include "Shader.h"
#include "../GameObjects/Actor.h"

namespace SGE::GRAPHICS {
    class Camera;
    class Renderer {
    public:
        Renderer();
        Renderer(std::shared_ptr<Shader> shader, std::shared_ptr<Camera> camera);
        ~Renderer();

        void initialize();
        void render(const std::shared_ptr<Camera>& camera, const std::vector<std::shared_ptr<SGE::GAMEOBJECTS::Actor>>& actors);
        void setCamera(const std::shared_ptr<Camera>& cam) { camera = cam; }

        // Shadow mapping methods
        void enableShadows(bool enable) { shadowsEnabled = enable; }
        bool areShadowsEnabled() const { return shadowsEnabled; }

        void setPBRMaterial(const std::shared_ptr<Shader>& shader,
                            const glm::vec3& albedo, float metallic, float roughness, float ao);
        void setPBRLight(const std::shared_ptr<Shader>& shader,
                         const std::string& lightName,
                         const glm::vec3& position,
                         const glm::vec3& color,
                         float intensity);

    private:
        std::shared_ptr<Shader> shader;
        std::shared_ptr<Camera> camera;
        bool shadowsEnabled;

        // PBR rendering helpers
        void setupPBRRendering(const std::shared_ptr<Shader>& shader);
        void setupShadowRendering(const std::shared_ptr<Shader>& shader,
                                 const glm::mat4& lightSpaceMatrix,
                                 const glm::vec3& lightPos);
    };
}

#endif // RENDERER_H

