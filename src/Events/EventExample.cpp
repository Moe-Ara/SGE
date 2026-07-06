//
// Created by Developer on [date].
//

#include "EventSystem.h"
#include "CollisionEvent.h"
#include "../Physics/BasePhysicsComponent.h"
#include "../GameObjects/Actor.h"
#include <iostream>

namespace SGE::EVENTS {
    // Example of how to set up and use the event system
    void setupExample() {
        // Create event system
        auto eventSystem = std::make_shared<EventSystem>("MainEventSystem");
        
        // Subscribe to collision events
        eventSystem->subscribe("collision", [](const IEvent& event) {
            if (auto collisionEvent = dynamic_cast<const CollisionEvent*>(&event)) {
                std::cout << "Collision detected between objects!" << std::endl;
                std::cout << "Event: " << collisionEvent->getName() << std::endl;
            }
        });
        
        // Create a simple test event
        auto testEvent = std::make_shared<Event>("test", "TestEvent");
        eventSystem->publish(*testEvent);
        
        // This is just an example - in a real application you'd use this with actors and physics components
    }
}