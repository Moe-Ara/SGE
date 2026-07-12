#ifndef SGE_SYSTEMS_LUASCRIPTSYSTEM_H
#define SGE_SYSTEMS_LUASCRIPTSYSTEM_H

#include "../ECS/ISystem.h"
#include <memory>
#include <string>
#include <unordered_map>

namespace SGE::SYSTEMS {

    class LuaScriptSystem final : public ECS::ISystem {
    public:
        struct Runtime;

        LuaScriptSystem();
        ~LuaScriptSystem() override;
        void update(entt::registry& registry, float deltaTime) override;
        void onWorldReset() override;

    private:
        std::unordered_map<entt::entity, std::unique_ptr<Runtime>> runtimes;
        Runtime* load(entt::registry& registry, entt::entity entity, const std::string& assetId);
    };

} // namespace SGE::SYSTEMS

#endif
