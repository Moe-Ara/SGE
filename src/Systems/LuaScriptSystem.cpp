#include "LuaScriptSystem.h"
#include "../Core/AssetLocator.h"
#include "../ECS/Components.h"

extern "C" {
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
}

#include <iostream>

namespace SGE::SYSTEMS {

    struct LuaScriptSystem::Runtime {
        struct Context {
            entt::registry* registry{};
            entt::entity entity{entt::null};
        } context;
        lua_State* state{};
        std::string assetId;
        bool failed{false};

        ~Runtime() { if (state) lua_close(state); }
    };

    namespace {
        using Runtime = LuaScriptSystem::Runtime;

        Runtime::Context* getContext(lua_State* state) {
            return static_cast<Runtime::Context*>(lua_touserdata(state, lua_upvalueindex(1)));
        }

        ECS::TransformComponent* getTransform(lua_State* state) {
            auto* context = getContext(state);
            return context && context->registry && context->registry->valid(context->entity)
                ? context->registry->try_get<ECS::TransformComponent>(context->entity)
                : nullptr;
        }

        int argumentOffset(lua_State* state) { return lua_istable(state, 1) ? 1 : 0; }

        int getPosition(lua_State* state) {
            const auto* transform = getTransform(state);
            if (!transform) return luaL_error(state, "entity has no Transform component");
            lua_pushnumber(state, transform->translation.x);
            lua_pushnumber(state, transform->translation.y);
            lua_pushnumber(state, transform->translation.z);
            return 3;
        }

        int setPosition(lua_State* state) {
            auto* transform = getTransform(state);
            if (!transform) return luaL_error(state, "entity has no Transform component");
            const int offset = argumentOffset(state);
            transform->translation = {
                static_cast<float>(luaL_checknumber(state, offset + 1)),
                static_cast<float>(luaL_checknumber(state, offset + 2)),
                static_cast<float>(luaL_checknumber(state, offset + 3))
            };
            return 0;
        }

        int translate(lua_State* state) {
            auto* transform = getTransform(state);
            if (!transform) return luaL_error(state, "entity has no Transform component");
            const int offset = argumentOffset(state);
            transform->translation += glm::vec3{
                static_cast<float>(luaL_checknumber(state, offset + 1)),
                static_cast<float>(luaL_checknumber(state, offset + 2)),
                static_cast<float>(luaL_checknumber(state, offset + 3))
            };
            return 0;
        }

        int log(lua_State* state) {
            std::cerr << "[Lua]";
            for (int index = 1; index <= lua_gettop(state); ++index) {
                std::size_t length{};
                const char* value = luaL_tolstring(state, index, &length);
                std::cerr << ' ' << std::string(value, length);
                lua_pop(state, 1);
            }
            std::cerr << '\n';
            return 0;
        }

        void bind(lua_State* state, const char* name, lua_CFunction function,
                  Runtime::Context* context) {
            lua_pushlightuserdata(state, context);
            lua_pushcclosure(state, function, 1);
            lua_setfield(state, -2, name);
        }

        bool invoke(lua_State* state, const char* name, float deltaTime, bool passDeltaTime) {
            lua_getglobal(state, name);
            if (lua_isnil(state, -1)) {
                lua_pop(state, 1);
                return true;
            }
            if (!lua_isfunction(state, -1)) {
                lua_pop(state, 1);
                std::cerr << "[Lua] '" << name << "' must be a function\n";
                return false;
            }
            if (passDeltaTime) lua_pushnumber(state, deltaTime);
            if (lua_pcall(state, passDeltaTime ? 1 : 0, 0, 0) == LUA_OK) return true;
            std::cerr << "[Lua] " << lua_tostring(state, -1) << '\n';
            lua_pop(state, 1);
            return false;
        }
    }

    LuaScriptSystem::LuaScriptSystem() = default;
    LuaScriptSystem::~LuaScriptSystem() = default;

    LuaScriptSystem::Runtime* LuaScriptSystem::load(
        entt::registry& registry, entt::entity entity, const std::string& assetId
    ) {
        auto runtime = std::make_unique<Runtime>();
        runtime->assetId = assetId;
        runtime->context = {&registry, entity};
        runtime->state = luaL_newstate();
        if (!runtime->state) return nullptr;

        lua_State* state = runtime->state;
        luaL_requiref(state, "_G", luaopen_base, 1); lua_pop(state, 1);
        lua_pushnil(state); lua_setglobal(state, "dofile");
        lua_pushnil(state); lua_setglobal(state, "loadfile");
        luaL_requiref(state, LUA_MATHLIBNAME, luaopen_math, 1); lua_pop(state, 1);
        luaL_requiref(state, LUA_STRLIBNAME, luaopen_string, 1); lua_pop(state, 1);
        luaL_requiref(state, LUA_TABLIBNAME, luaopen_table, 1); lua_pop(state, 1);

        lua_newtable(state);
        const auto* identity = registry.try_get<ECS::SceneIdentityComponent>(entity);
        lua_pushinteger(state, static_cast<lua_Integer>(identity ? identity->id : 0u));
        lua_setfield(state, -2, "id");
        bind(state, "get_position", getPosition, &runtime->context);
        bind(state, "set_position", setPosition, &runtime->context);
        bind(state, "translate", translate, &runtime->context);
        lua_setglobal(state, "entity");

        lua_newtable(state);
        lua_pushcfunction(state, log);
        lua_setfield(state, -2, "log");
        lua_setglobal(state, "sge");

        try {
            const std::string path = CORE::AssetLocator::resolve(assetId).string();
            if (luaL_loadfile(state, path.c_str()) != LUA_OK || lua_pcall(state, 0, 0, 0) != LUA_OK) {
                std::cerr << "[Lua] " << assetId << ": " << lua_tostring(state, -1) << '\n';
                lua_pop(state, 1);
                runtime->failed = true;
            } else if (!invoke(state, "start", 0.0f, false)) {
                runtime->failed = true;
            }
        } catch (const std::exception& error) {
            std::cerr << "[Lua] " << assetId << ": " << error.what() << '\n';
            runtime->failed = true;
        }

        Runtime* result = runtime.get();
        runtimes[entity] = std::move(runtime);
        return result;
    }

    void LuaScriptSystem::update(entt::registry& registry, float deltaTime) {
        auto view = registry.view<ECS::ScriptComponent>();
        for (const auto entity : view) {
            const auto& script = view.get<ECS::ScriptComponent>(entity);
            if (!script.enabled || script.assetId.empty()) continue;
            const auto found = runtimes.find(entity);
            Runtime* runtime = found == runtimes.end() || found->second->assetId != script.assetId
                ? load(registry, entity, script.assetId) : found->second.get();
            if (runtime && !runtime->failed && !invoke(runtime->state, "update", deltaTime, true)) {
                runtime->failed = true;
            }
        }
    }

    void LuaScriptSystem::onWorldReset() { runtimes.clear(); }

} // namespace SGE::SYSTEMS
