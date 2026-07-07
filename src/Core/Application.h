#ifndef GLCPP_APPLICATION_H
#define GLCPP_APPLICATION_H

#include <entt/entt.hpp>
#include <memory>
#include <vector>
#include "../ECS/ISystem.h"
#include "../Input/InputHandler.h"
#include "../Events/EventSystem.h"
#include "../Editor/EditorUI.h"

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

        entt::registry registry;
        std::shared_ptr<SGE::INPUT::InputHandler> inputHandler;
        std::shared_ptr<SGE::EVENTS::EventSystem> eventSystem;
        SGE::EVENTS::SubscriptionId collisionSubscription{0};
        std::vector<std::unique_ptr<SGE::ECS::ISystem>> systems;
        std::unique_ptr<SGE::EDITOR::EditorUI> editorUI;
    };
}

#endif //GLCPP_APPLICATION_H
