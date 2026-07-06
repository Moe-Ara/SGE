//
// Physics System Test
//

#include <iostream>
#include "../src/Physics/PhysicsEngine.h"
#include "../src/GameObjects/Actor.h"
#include "../src/Events/EventSystem.h"

int main() {
    std::cout << "Testing Physics System..." << std::endl;
    
    // Create physics engine
    PhysicsEngine engine;
    
    // Create event system
    EventSystem events;
    
    // Set up event system for physics
    engine.setEventSystem(&events);
    
    // Create test actors
    Actor actor1("Actor1");
    Actor actor2("Actor2");
    
    // Add actors to engine
    engine.addActor(&actor1);
    engine.addActor(&actor2);
    
    std::cout << "Physics system initialized successfully!" << std::endl;
    std::cout << "Number of actors: " << engine.getActorCount() << std::endl;
    
    // Run a simple simulation step
    engine.update(0.016f); // 60 FPS
    
    std::cout << "Physics test completed successfully!" << std::endl;
    
    return 0;
}