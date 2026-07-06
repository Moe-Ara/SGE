# Physics and Event System Implementation

## Overview

This document describes the complete implementation of the physics engine and event system for the SGE game engine. The system provides robust collision detection, physics simulation, and a decoupled event communication architecture.

## Architecture Design

### Core Components

1. **Physics Engine**
   - `PhysicsEngine`: Main simulation manager coordinating all physics operations
   - `BasePhysicsComponent`: Base class for physics properties (mass, velocity, acceleration)
   - `SimpleCollisionSystem`: Collision detection using BVH (Bounding Volume Hierarchy) 
   - `BVH`: Spatial partitioning data structure for efficient collision queries
   - `IPhysicsComponent`: Interface for physics components to be attached to GameObjects

2. **Event System**
   - `EventSystem`: Central event management with publish/subscribe pattern
   - `IEventSystem`: Interface for event handling system
   - `CollisionEvent`: Specialized event type for collision notifications
   - `Event`: Base event class for custom events
   - `EventQueue`: Thread-safe queue for event processing

3. **Integration Points**
   - `GameObject`: Enhanced with physics component support
   - `Actor`: Inherits from GameObject with physics capabilities
   - `Transform`: Integrated with physics calculations

## Key Features Implemented

### Physics Simulation
1. **Basic Physics Properties**
   - Mass, velocity, acceleration tracking
   - Position and rotation integration
   - Force application and Newtonian physics
   - Time-based physics updates with delta time handling

2. **Collision Detection**
   - AABB (Axis-Aligned Bounding Box) collision detection
   - BVH spatial partitioning for efficient queries
   - Collision response handling
   - Support for different physics materials

3. **Physics Integration**
   - Seamless integration with existing Transform system
   - Support for multiple physics components per entity
   - Physics updates coordinated through central engine
   - Force and velocity calculations with proper integration

### Event System
1. **Event Architecture**
   - Publish/subscribe pattern for decoupled communication
   - Thread-safe event processing
   - Custom event types with type safety
   - Automatic collision event publishing

2. **Integration Points**
   - Physics engine automatically publishes collision events
   - Events can trigger other systems (rendering, game logic)
   - Easy extension for custom event types
   - Integration with existing game loop architecture

## Implementation Details

### PhysicsEngine Class
```cpp
class PhysicsEngine {
private:
    std::vector<std::shared_ptr<IPhysicsComponent>> physicsComponents;
    std::unique_ptr<SimpleCollisionSystem> collisionSystem;
    
public:
    void update(float deltaTime);
    void addPhysicsComponent(std::shared_ptr<IPhysicsComponent> component);
    void removePhysicsComponent(std::shared_ptr<IPhysicsComponent> component);
};
```

### BasePhysicsComponent Class
```cpp
class BasePhysicsComponent : public IPhysicsComponent {
private:
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 acceleration;
    float mass;
    
public:
    void update(float deltaTime) override;
    void applyForce(const glm::vec3& force);
    // ... other physics methods
};
```

### Event System Architecture
```cpp
class EventSystem {
private:
    std::unordered_map<std::type_index, std::vector<EventCallback>> subscribers;
    
public:
    template<typename T>
    void subscribe(EventCallback callback);
    
    template<typename T>
    void publish(const T& event);
    
    void processEvents();
};
```

## Usage Examples

### Setting up Physics Component
```cpp
// Create physics component
auto physics = std::make_shared<BasePhysicsComponent>();
physics->setMass(10.0f);
physics->setVelocity(glm::vec3(0, 0, 5));

// Add to entity
entity.addComponent(physics);
```

### Event Handling
```cpp
// Subscribe to collision events
eventSystem.subscribe<CollisionEvent>([](const CollisionEvent& event) {
    // Handle collision response
    std::cout << "Collision detected!" << std::endl;
});

// Subscribe to custom events
eventSystem.subscribe<MyCustomEvent>([](const MyCustomEvent& event) {
    // Handle custom event logic
});
```

### Physics Simulation Loop
```cpp
void update(float deltaTime) {
    physicsEngine.update(deltaTime);
    eventSystem.processEvents();
}
```

## Integration Benefits

1. **Modularity**: Physics components can be added/removed independently without affecting other systems
2. **Extensibility**: Easy to add new physics types or event types through inheritance and templates
3. **Performance**: BVH for efficient collision detection with O(log n) lookup time
4. **Decoupling**: Event system reduces direct dependencies between components
5. **Maintainability**: Clear separation of concerns with well-defined interfaces

## File Structure

The implementation follows this file structure:

```
src/
├── physics/
│   ├── PhysicsEngine.cpp
│   ├── PhysicsEngine.h
│   ├── BasePhysicsComponent.cpp
│   ├── BasePhysicsComponent.h
│   ├── IPhysicsComponent.h
│   └── SimpleCollisionSystem.cpp
│   └── SimpleCollisionSystem.h
├── events/
│   ├── EventSystem.cpp
│   ├── EventSystem.h
│   ├── CollisionEvent.h
│   └── Event.h
└── game/
    ├── Actor.cpp
    ├── Actor.h
    └── GameObject.cpp
    └── GameObject.h
```

## Future Enhancements

1. **Advanced Physics Features**
   - Rigid body dynamics simulation
   - Joint and constraint systems
   - Advanced collision shapes (spheres, capsules)
   - Physics material properties

2. **Event System Improvements**
   - Event prioritization
   - Asynchronous event processing
   - Event filtering and routing
   - Better error handling for subscriptions

3. **Performance Optimizations**
   - Spatial partitioning improvements
   - Physics culling systems
   - Multi-threaded physics simulation
   - Memory management optimizations

## Testing Strategy

The system includes unit tests for:
- Physics component behavior
- Collision detection algorithms
- Event subscription and publishing
- Integration between physics and events
- Performance benchmarks

## Dependencies

This implementation requires:
- C++17 compiler support
- OpenGL for rendering (for visualization)
- GLM for mathematical operations
- Standard C++ libraries

## License

MIT License - see LICENSE file for details.