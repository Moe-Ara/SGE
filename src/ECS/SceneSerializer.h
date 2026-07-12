#ifndef SGE_ECS_SCENESERIALIZER_H
#define SGE_ECS_SCENESERIALIZER_H

#include "Components.h"
#include <filesystem>
#include <functional>
#include <string>

namespace SGE::ECS {

    struct SceneAssetResolver {
        std::function<std::shared_ptr<GAMEOBJECTS::Model>(const std::string&)> model;
        std::function<std::shared_ptr<GRAPHICS::Texture>(const std::string&)> texture;
    };

    class SceneSerializer {
    public:
        static constexpr int CurrentVersion = 1;

        static std::string serialize(entt::registry& registry);
        static entt::registry deserialize(const std::string& document,
                                          const SceneAssetResolver& resolver = {});
        static void save(entt::registry& registry, const std::filesystem::path& path);
        static entt::registry load(const std::filesystem::path& path,
                                   const SceneAssetResolver& resolver = {});
        static entt::registry cloneRuntime(entt::registry& source);
    };

} // namespace SGE::ECS

#endif // SGE_ECS_SCENESERIALIZER_H
