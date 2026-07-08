#ifndef GLCPP_APPLICATION_H
#define GLCPP_APPLICATION_H

#include <entt/entt.hpp>
#include <memory>
#include <vector>
#include "../ECS/ISystem.h"
#include "../Input/InputHandler.h"
#include "../Events/EventSystem.h"
#include "../Editor/EditorUI.h"
#include "EngineMode.h"

namespace SGE::CORE {
    class Application {
    public:
        Application();
        ~Application();

        void run();

    private:
        void setup();
        void buildScene();
        void gameLoop();
        void cleanup();

        void togglePlayMode();
        void enterPlayMode();
        void exitPlayMode();

        entt::registry registry;
        std::shared_ptr<SGE::INPUT::InputHandler> inputHandler;
        std::shared_ptr<SGE::EVENTS::EventSystem> eventSystem;
        SGE::EVENTS::SubscriptionId collisionSubscription{0};

        // Simulation systems (input/physics/collision) only run in Play mode.
        // Presentation systems (camera/render/skybox) run in both modes so the
        // scene stays visible and navigable while inspecting.
        std::vector<std::unique_ptr<SGE::ECS::ISystem>> simulationSystems;
        std::vector<std::unique_ptr<SGE::ECS::ISystem>> presentationSystems;
        std::unique_ptr<SGE::EDITOR::EditorUI> editorUI;

        EngineMode mode{EngineMode::Inspection};
        // Registry contents as they were right before entering Play mode, so
        // Stop can discard whatever Play did and restore the edited scene -
        // entt::registry has no copy constructor, so this is a manual clone.
        entt::registry prePlaySnapshot;
    };
}

#endif //GLCPP_APPLICATION_H
