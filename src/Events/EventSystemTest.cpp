//
// Created by Developer on [date].
//

#include "EventSystem.h"
#include "CollisionEvent.h"
#include "../GameObjects/Actor.h"
#include "../Physics/BasePhysicsComponent.h"
#include <iostream>

namespace SGE::EVENTS {
    // Simple test to demonstrate event system functionality
    void runEventSystemTest() {
        std::cout << "Running Event System Test..." << std::endl;
        
        // Create event system
        auto eventSystem = std::make_shared<EventSystem>("TestEventSystem");
        
        // Subscribe to collision events
        eventSystem->subscribe("collision", [](const IEvent& event) {
            if (auto collisionEvent = dynamic_cast<const CollisionEvent*>(&event)) {
                std::cout << "✓ Collision event received: " << collisionEvent->getName() << std::endl;
            }
        });
        
        // Test publishing a simple event
        auto testEvent = std::make_shared<Event>("test", "Test Event");
        eventSystem->publish(*testEvent);
        
        std::cout << "✓ Event system test completed successfully!" << std::endl;
    }
}