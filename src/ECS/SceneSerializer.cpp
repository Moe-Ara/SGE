#include "SceneSerializer.h"
#include "../Core/AssetLocator.h"
#include "../../external/nlohmann/json.hpp"
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iterator>
#include <limits>
#include <stdexcept>
#include <unordered_map>
#include <unordered_set>

namespace SGE::ECS {

    namespace {
        using json = nlohmann::json;
        constexpr std::size_t kMaximumEntities = 1'000'000u;
        constexpr std::uintmax_t kMaximumSceneBytes = 64u * 1024u * 1024u;

        json vec3(const glm::vec3& value) {
            return json::array({value.x, value.y, value.z});
        }

        glm::vec3 readVec3(const json& value) {
            if (!value.is_array() || value.size() != 3u) {
                throw std::runtime_error("Expected a three-element vector");
            }
            return {value.at(0).get<float>(), value.at(1).get<float>(), value.at(2).get<float>()};
        }

        json quaternion(const glm::quat& value) {
            return json::array({value.w, value.x, value.y, value.z});
        }

        glm::quat readQuaternion(const json& value) {
            if (!value.is_array() || value.size() != 4u) {
                throw std::runtime_error("Expected a four-element quaternion");
            }
            const glm::quat result{value.at(0).get<float>(), value.at(1).get<float>(),
                                   value.at(2).get<float>(), value.at(3).get<float>()};
            const float length = glm::length(result);
            if (!std::isfinite(length) || length <= std::numeric_limits<float>::epsilon()) {
                throw std::runtime_error("Scene contains an invalid rotation quaternion");
            }
            return glm::normalize(result);
        }

        bool isRegisteredComponent(entt::id_type id) {
            return id == entt::type_hash<SceneIdentityComponent>::value() ||
                   id == entt::type_hash<TransformComponent>::value() ||
                   id == entt::type_hash<MeshComponent>::value() ||
                   id == entt::type_hash<MaterialComponent>::value() ||
                   id == entt::type_hash<RigidBodyComponent>::value() ||
                   id == entt::type_hash<SphereColliderComponent>::value() ||
                   id == entt::type_hash<LightComponent>::value() ||
                   id == entt::type_hash<CameraComponent>::value() ||
                   id == entt::type_hash<FreeCameraComponent>::value() ||
                   id == entt::type_hash<ThirdPersonFollowComponent>::value() ||
                   id == entt::type_hash<PlayerControllerComponent>::value() ||
                   id == entt::type_hash<TagComponent>::value();
        }

        std::unordered_map<entt::entity, std::uint64_t> ensureSceneIds(entt::registry& registry) {
            std::unordered_map<entt::entity, std::uint64_t> ids;
            std::unordered_set<std::uint64_t> used;
            std::uint64_t nextId = 1;

            auto existing = registry.view<SceneIdentityComponent>();
            for (const auto entity : existing) {
                const std::uint64_t id = existing.get<SceneIdentityComponent>(entity).id;
                if (id == 0u || !used.insert(id).second) {
                    throw std::runtime_error("Scene entity IDs must be non-zero and unique");
                }
                ids.emplace(entity, id);
                if (id == std::numeric_limits<std::uint64_t>::max()) {
                    throw std::runtime_error("Scene entity ID space is exhausted");
                }
                nextId = std::max(nextId, id + 1u);
            }

            for (const auto [entity] : registry.storage<entt::entity>().each()) {
                if (!ids.contains(entity)) {
                    while (used.contains(nextId)) ++nextId;
                    if (nextId == std::numeric_limits<std::uint64_t>::max()) {
                        throw std::runtime_error("Scene entity ID space is exhausted");
                    }
                    registry.emplace<SceneIdentityComponent>(entity, nextId);
                    ids.emplace(entity, nextId);
                    used.insert(nextId++);
                }
            }
            return ids;
        }

        std::uint64_t referencedId(entt::entity target,
                                   const std::unordered_map<entt::entity, std::uint64_t>& ids) {
            if (target == entt::null) return 0u;
            const auto found = ids.find(target);
            if (found == ids.end()) {
                throw std::runtime_error("Scene contains an invalid entity reference");
            }
            return found->second;
        }

        void rejectUnknownComponents(const json& components) {
            static const std::unordered_set<std::string> known{
                "Tag", "Transform", "Mesh", "Material", "RigidBody", "SphereCollider",
                "Light", "Camera", "FreeCamera", "ThirdPersonFollow", "PlayerController"
            };
            for (const auto& [name, value] : components.items()) {
                (void)value;
                if (!known.contains(name)) {
                    throw std::runtime_error("Unsupported scene component: " + name);
                }
            }
        }

        template<typename Resource>
        void addResource(std::unordered_map<std::string, std::shared_ptr<Resource>>& resources,
                         const std::string& id,
                         const std::shared_ptr<Resource>& resource) {
            if (id.empty() || !resource) return;
            const auto [position, inserted] = resources.emplace(id, resource);
            if (!inserted && position->second != resource) {
                throw std::runtime_error("Asset ID refers to multiple runtime resources: " + id);
            }
        }
    }

    std::string SceneSerializer::serialize(entt::registry& registry) {
        for (const auto [id, storage] : registry.storage()) {
            if (storage.size() != 0u && !isRegisteredComponent(id)) {
                throw std::runtime_error("Cannot serialize an unregistered component type");
            }
        }

        const auto ids = ensureSceneIds(registry);
        json root{{"format", "SGE.Scene"}, {"version", CurrentVersion}, {"entities", json::array()}};

        for (const auto [entity] : registry.storage<entt::entity>().each()) {
            json components = json::object();

            if (const auto* value = registry.try_get<TagComponent>(entity)) {
                components["Tag"] = {{"name", value->name}};
            }
            if (const auto* value = registry.try_get<TransformComponent>(entity)) {
                components["Transform"] = {
                    {"translation", vec3(value->translation)},
                    {"rotation", quaternion(value->rotation)},
                    {"scale", vec3(value->scale)}
                };
            }
            if (auto* value = registry.try_get<MeshComponent>(entity)) {
                if (value->assetId.empty() && value->model) {
                    value->assetId = "runtime:model:" + std::to_string(ids.at(entity));
                }
                components["Mesh"] = {{"asset", value->assetId}};
            }
            if (auto* value = registry.try_get<MaterialComponent>(entity)) {
                if (value->albedoTextureAssetId.empty() && value->albedoTexture) {
                    value->albedoTextureAssetId =
                        "runtime:texture:" + std::to_string(ids.at(entity));
                }
                components["Material"] = {
                    {"albedo", vec3(value->albedo)}, {"metallic", value->metallic},
                    {"roughness", value->roughness}, {"ao", value->ao},
                    {"albedoTexture", value->albedoTextureAssetId},
                    {"phong", {
                        {"ambient", vec3(value->phong.ambient)},
                        {"diffuse", vec3(value->phong.diffuse)},
                        {"specular", vec3(value->phong.specular)},
                        {"shininess", value->phong.shininess},
                        {"hasDiffuseTexture", value->phong.hasDiffuseTexture},
                        {"hasSpecularTexture", value->phong.hasSpecularTexture},
                        {"hasNormalTexture", value->phong.hasNormalTexture}
                    }}
                };
            }
            if (const auto* value = registry.try_get<RigidBodyComponent>(entity)) {
                components["RigidBody"] = {
                    {"velocity", vec3(value->velocity)}, {"acceleration", vec3(value->acceleration)},
                    {"force", vec3(value->force)}, {"mass", value->mass},
                    {"restitution", value->restitution}, {"static", value->isStatic},
                    {"enabled", value->enabled}
                };
            }
            if (const auto* value = registry.try_get<SphereColliderComponent>(entity)) {
                components["SphereCollider"] = {{"radius", value->radius}};
            }
            if (const auto* value = registry.try_get<LightComponent>(entity)) {
                const auto& light = value->light;
                components["Light"] = {
                    {"position", vec3(light.position)}, {"color", vec3(light.color)},
                    {"intensity", light.intensity}, {"constant", light.constant},
                    {"linear", light.linear}, {"quadratic", light.quadratic},
                    {"cutOff", light.cutOff}, {"outerCutOff", light.outerCutOff},
                    {"castsShadows", light.castsShadows}
                };
            }
            if (const auto* value = registry.try_get<CameraComponent>(entity)) {
                components["Camera"] = {
                    {"primary", value->isPrimary}, {"fovYRadians", value->fovYRadians},
                    {"nearPlane", value->nearPlane}, {"farPlane", value->farPlane}
                };
            }
            if (const auto* value = registry.try_get<FreeCameraComponent>(entity)) {
                components["FreeCamera"] = {
                    {"movementSpeed", value->movementSpeed},
                    {"sprintMultiplier", value->sprintMultiplier},
                    {"mouseSensitivity", value->mouseSensitivity},
                    {"rotationResponsiveness", value->rotationResponsiveness}, {"yaw", value->yaw},
                    {"pitch", value->pitch}, {"enabled", value->enabled}
                };
            }
            if (const auto* value = registry.try_get<ThirdPersonFollowComponent>(entity)) {
                components["ThirdPersonFollow"] = {
                    {"target", referencedId(value->target, ids)}, {"offset", vec3(value->offset)},
                    {"yaw", value->yaw}, {"pitch", value->pitch},
                    {"mouseSensitivity", value->mouseSensitivity}, {"enabled", value->enabled}
                };
            }
            if (const auto* value = registry.try_get<PlayerControllerComponent>(entity)) {
                components["PlayerController"] = {
                    {"movementSpeed", value->movementSpeed}, {"jumpSpeed", value->jumpSpeed},
                    {"grounded", value->grounded}
                };
            }

            root["entities"].push_back({{"id", ids.at(entity)}, {"components", components}});
        }
        return root.dump(2);
    }

    entt::registry SceneSerializer::deserialize(const std::string& document,
                                                const SceneAssetResolver& resolver) {
        try {
            const json root = json::parse(document);
            if (root.at("format") != "SGE.Scene") {
                throw std::runtime_error("Not an SGE scene document");
            }
            const int version = root.at("version").get<int>();
            if (version != CurrentVersion) {
                throw std::runtime_error("Unsupported SGE scene version: " + std::to_string(version));
            }
            const json& entities = root.at("entities");
            if (!entities.is_array() || entities.size() > kMaximumEntities) {
                throw std::runtime_error("Scene entity list is invalid or too large");
            }

            entt::registry registry;
            std::unordered_map<std::uint64_t, entt::entity> entityById;
            entityById.reserve(entities.size());
            for (const auto& serialized : entities) {
                const std::uint64_t id = serialized.at("id").get<std::uint64_t>();
                if (id == 0u || entityById.contains(id)) {
                    throw std::runtime_error("Scene entity IDs must be non-zero and unique");
                }
                const entt::entity entity = registry.create();
                registry.emplace<SceneIdentityComponent>(entity, id);
                entityById.emplace(id, entity);
            }

            for (const auto& serialized : entities) {
                const entt::entity entity = entityById.at(serialized.at("id").get<std::uint64_t>());
                const json& components = serialized.at("components");
                if (!components.is_object()) throw std::runtime_error("Entity components must be an object");
                rejectUnknownComponents(components);

                if (const auto found = components.find("Tag"); found != components.end()) {
                    registry.emplace<TagComponent>(entity, found->at("name").get<std::string>());
                }
                if (const auto found = components.find("Transform"); found != components.end()) {
                    auto& value = registry.emplace<TransformComponent>(entity);
                    value.translation = readVec3(found->at("translation"));
                    value.rotation = readQuaternion(found->at("rotation"));
                    value.scale = readVec3(found->at("scale"));
                }
                if (const auto found = components.find("Mesh"); found != components.end()) {
                    auto& value = registry.emplace<MeshComponent>(entity);
                    value.assetId = found->at("asset").get<std::string>();
                    if (resolver.model && !value.assetId.empty()) value.model = resolver.model(value.assetId);
                }
                if (const auto found = components.find("Material"); found != components.end()) {
                    auto& value = registry.emplace<MaterialComponent>(entity);
                    value.albedo = readVec3(found->at("albedo"));
                    value.metallic = found->at("metallic").get<float>();
                    value.roughness = found->at("roughness").get<float>();
                    value.ao = found->at("ao").get<float>();
                    value.albedoTextureAssetId = found->at("albedoTexture").get<std::string>();
                    if (resolver.texture && !value.albedoTextureAssetId.empty()) {
                        value.albedoTexture = resolver.texture(value.albedoTextureAssetId);
                    }
                    const json& phong = found->at("phong");
                    value.phong.ambient = readVec3(phong.at("ambient"));
                    value.phong.diffuse = readVec3(phong.at("diffuse"));
                    value.phong.specular = readVec3(phong.at("specular"));
                    value.phong.shininess = phong.at("shininess").get<float>();
                    value.phong.hasDiffuseTexture = phong.at("hasDiffuseTexture").get<bool>();
                    value.phong.hasSpecularTexture = phong.at("hasSpecularTexture").get<bool>();
                    value.phong.hasNormalTexture = phong.at("hasNormalTexture").get<bool>();
                    if (value.metallic < 0.0f || value.metallic > 1.0f ||
                        value.roughness <= 0.0f || value.roughness > 1.0f ||
                        value.ao < 0.0f || value.ao > 1.0f) {
                        throw std::runtime_error("Scene contains invalid PBR material values");
                    }
                }
                if (const auto found = components.find("RigidBody"); found != components.end()) {
                    auto& value = registry.emplace<RigidBodyComponent>(entity);
                    value.velocity = readVec3(found->at("velocity"));
                    value.acceleration = readVec3(found->at("acceleration"));
                    value.force = readVec3(found->at("force"));
                    value.mass = found->at("mass").get<float>();
                    value.restitution = found->at("restitution").get<float>();
                    value.isStatic = found->at("static").get<bool>();
                    value.enabled = found->at("enabled").get<bool>();
                    if (!value.isStatic && (!std::isfinite(value.mass) || value.mass <= 0.0f)) {
                        throw std::runtime_error("Scene contains an invalid dynamic rigid-body mass");
                    }
                }
                if (const auto found = components.find("SphereCollider"); found != components.end()) {
                    const float radius = found->at("radius").get<float>();
                    if (!std::isfinite(radius) || radius < 0.0f) {
                        throw std::runtime_error("Scene contains an invalid sphere radius");
                    }
                    registry.emplace<SphereColliderComponent>(entity, radius);
                }
                if (const auto found = components.find("Light"); found != components.end()) {
                    auto& light = registry.emplace<LightComponent>(entity).light;
                    light.position = readVec3(found->at("position"));
                    light.color = readVec3(found->at("color"));
                    light.intensity = found->at("intensity").get<float>();
                    light.constant = found->at("constant").get<float>();
                    light.linear = found->at("linear").get<float>();
                    light.quadratic = found->at("quadratic").get<float>();
                    light.cutOff = found->at("cutOff").get<float>();
                    light.outerCutOff = found->at("outerCutOff").get<float>();
                    light.castsShadows = found->at("castsShadows").get<bool>();
                }
                if (const auto found = components.find("Camera"); found != components.end()) {
                    auto& value = registry.emplace<CameraComponent>(entity);
                    value.isPrimary = found->at("primary").get<bool>();
                    value.fovYRadians = found->at("fovYRadians").get<float>();
                    value.nearPlane = found->at("nearPlane").get<float>();
                    value.farPlane = found->at("farPlane").get<float>();
                    value.camera.setPerspectiveProjection(
                        value.fovYRadians, 1.0f, value.nearPlane, value.farPlane
                    );
                }
                if (const auto found = components.find("FreeCamera"); found != components.end()) {
                    auto& value = registry.emplace<FreeCameraComponent>(entity);
                    value.movementSpeed = found->at("movementSpeed").get<float>();
                    value.sprintMultiplier = found->at("sprintMultiplier").get<float>();
                    value.mouseSensitivity = found->at("mouseSensitivity").get<float>();
                    value.rotationResponsiveness = found->value(
                        "rotationResponsiveness",
                        value.rotationResponsiveness
                    );
                    value.yaw = found->at("yaw").get<float>();
                    value.pitch = found->at("pitch").get<float>();
                    value.enabled = found->at("enabled").get<bool>();
                }
                if (const auto found = components.find("ThirdPersonFollow"); found != components.end()) {
                    auto& value = registry.emplace<ThirdPersonFollowComponent>(entity);
                    const std::uint64_t target = found->at("target").get<std::uint64_t>();
                    if (target != 0u) {
                        const auto targetEntity = entityById.find(target);
                        if (targetEntity == entityById.end()) {
                            throw std::runtime_error("Scene contains an unresolved entity reference");
                        }
                        value.target = targetEntity->second;
                    }
                    value.offset = readVec3(found->at("offset"));
                    value.yaw = found->at("yaw").get<float>();
                    value.pitch = found->at("pitch").get<float>();
                    value.mouseSensitivity = found->at("mouseSensitivity").get<float>();
                    value.enabled = found->at("enabled").get<bool>();
                }
                if (const auto found = components.find("PlayerController"); found != components.end()) {
                    auto& value = registry.emplace<PlayerControllerComponent>(entity);
                    value.movementSpeed = found->at("movementSpeed").get<float>();
                    value.jumpSpeed = found->at("jumpSpeed").get<float>();
                    value.grounded = found->at("grounded").get<bool>();
                }
            }

            std::size_t primaryCameraCount = 0;
            for (const auto entity : registry.view<CameraComponent>()) {
                if (registry.get<CameraComponent>(entity).isPrimary) ++primaryCameraCount;
            }
            if (primaryCameraCount > 1u) {
                throw std::runtime_error("Scene contains more than one primary camera");
            }
            return registry;
        } catch (const nlohmann::json::exception& error) {
            throw std::runtime_error(std::string{"Invalid SGE scene document: "} + error.what());
        }
    }

    void SceneSerializer::save(entt::registry& registry, const std::filesystem::path& path) {
        if (path.empty()) throw std::invalid_argument("Scene path cannot be empty");
        const std::string document = serialize(registry);
        const std::filesystem::path temporary = path.string() + ".tmp";
        {
            std::ofstream output(temporary, std::ios::binary | std::ios::trunc);
            if (!output) throw std::runtime_error("Unable to write scene: " + temporary.string());
            output.write(document.data(), static_cast<std::streamsize>(document.size()));
            output.flush();
            if (!output) throw std::runtime_error("Failed while writing scene: " + temporary.string());
        }
        const std::filesystem::path backup = path.string() + ".bak";
        std::error_code error;
        const bool replacing = std::filesystem::exists(path);
        if (replacing) {
            std::filesystem::remove(backup, error);
            error.clear();
            std::filesystem::rename(path, backup, error);
            if (error) {
                std::filesystem::remove(temporary);
                throw std::runtime_error("Unable to back up existing scene: " + error.message());
            }
        }

        std::filesystem::rename(temporary, path, error);
        if (error) {
            if (replacing) {
                std::error_code restoreError;
                std::filesystem::rename(backup, path, restoreError);
            }
            std::filesystem::remove(temporary);
            throw std::runtime_error("Unable to replace scene file: " + error.message());
        }
        if (replacing) std::filesystem::remove(backup);
    }

    entt::registry SceneSerializer::load(const std::filesystem::path& path,
                                         const SceneAssetResolver& resolver) {
        if (!std::filesystem::is_regular_file(path)) {
            throw std::runtime_error("Scene does not exist: " + path.string());
        }
        if (std::filesystem::file_size(path) > kMaximumSceneBytes) {
            throw std::runtime_error("Scene file exceeds the supported size limit");
        }
        std::ifstream input(path, std::ios::binary);
        const std::string document{std::istreambuf_iterator<char>{input},
                                   std::istreambuf_iterator<char>{}};
        return deserialize(document, resolver);
    }

    entt::registry SceneSerializer::cloneRuntime(entt::registry& source) {
        const std::string document = serialize(source);
        std::unordered_map<std::string, std::shared_ptr<GAMEOBJECTS::Model>> models;
        std::unordered_map<std::string, std::shared_ptr<GRAPHICS::Texture>> textures;
        for (const auto entity : source.view<MeshComponent>()) {
            const auto& mesh = source.get<MeshComponent>(entity);
            addResource(models, mesh.assetId, mesh.model);
        }
        for (const auto entity : source.view<MaterialComponent>()) {
            const auto& material = source.get<MaterialComponent>(entity);
            addResource(textures, material.albedoTextureAssetId, material.albedoTexture);
        }

        SceneAssetResolver resolver;
        resolver.model = [&models](const std::string& id) {
            const auto found = models.find(id);
            return found == models.end() ? std::shared_ptr<GAMEOBJECTS::Model>{} : found->second;
        };
        resolver.texture = [&textures](const std::string& id) {
            const auto found = textures.find(id);
            return found == textures.end() ? std::shared_ptr<GRAPHICS::Texture>{} : found->second;
        };
        return deserialize(document, resolver);
    }

} // namespace SGE::ECS
