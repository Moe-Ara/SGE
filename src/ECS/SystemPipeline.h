#ifndef SGE_ECS_SYSTEMPIPELINE_H
#define SGE_ECS_SYSTEMPIPELINE_H

#include "ISystem.h"
#include <cstddef>
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

namespace SGE::ECS {

    // Compact owner for an ordered system phase. Systems allocate once when added;
    // update performs a contiguous pointer walk with no frame-local allocation.
    class SystemPipeline {
    public:
        explicit SystemPipeline(std::size_t expectedSystems = 8) {
            systems.reserve(expectedSystems);
        }

        template<typename System, typename... Args>
        System& emplace(Args&&... args) {
            static_assert(std::is_base_of_v<ISystem, System>);
            auto system = std::make_unique<System>(std::forward<Args>(args)...);
            System& result = *system;
            systems.push_back(std::move(system));
            return result;
        }

        void update(entt::registry& registry, float deltaTime) {
            for (const auto& system : systems) {
                system->update(registry, deltaTime);
            }
        }

        void onWorldReset() {
            for (const auto& system : systems) {
                system->onWorldReset();
            }
        }

        void clear() noexcept { systems.clear(); }
        [[nodiscard]] std::size_t size() const noexcept { return systems.size(); }

    private:
        std::vector<std::unique_ptr<ISystem>> systems;
    };

} // namespace SGE::ECS

#endif // SGE_ECS_SYSTEMPIPELINE_H
