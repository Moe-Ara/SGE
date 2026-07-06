# SGE Game Engine - Physics and Event System Technical Specification

## Document Overview

This document provides a complete technical specification for the physics engine and event system implemented in the Simple Game Engine (SGE). The system delivers robust physics simulation capabilities with an integrated event-driven architecture that supports modern game development requirements.

## 1. System Architecture

### 1.1 Core Design Principles

The implementation follows these fundamental design principles:

- **Modularity**: Independent components that can be developed and tested separately
- **Integration**: Seamless connection with existing GameObject/Actor architecture  
- **Performance**: Optimized algorithms for real-time game simulation
- **Extensibility**: Flexible design supporting future enhancements
- **Decoupling**: Event system reduces tight coupling between components

### 1.2 System Components

#### Physics Engine Components
1. **PhysicsEngine** - Central coordinator managing all physics operations
2. **BasePhysicsComponent** - Implements fundamental physics properties
3. **SimpleCollisionSystem** - Collision detection and response handling
4. **BVH (Bounding Volume Hierarchy)** - Spatial partitioning for efficient queries
5. **IPhysicsComponent** - Interface defining required physics behaviors

#### Event System Components  
1. **EventSystem** - Publish/subscribe pattern for component communication
2. **IEventSystem** - Interface for event handling contract
3. **CollisionEvent** - Specialized event type for collision notifications
4. **Event** - Base class for all custom events

## 2. Detailed Component Specifications

### 2.1 PhysicsEngine Class

```cpp
class PhysicsEngine {
private:
    std::vector<std::shared_ptr<IPhysicsComponent>> physicsComponents;
    std::unique_ptr<SimpleCollisionSystem> collisionSystem;
    
public:
    void update(float deltaTime);
    void addPhysicsComponent(std::shared_ptr<IPhysicsComponent> component);
    void removePhysicsComponent(std::shared_ptr<IPhysicsComponent> component);
    void setGravity(const glm::vec3& gravity);
    glm::vec3 getGravity() const;
};
```

**Key Responsibilities:**
- Coordinate physics updates for all active components
- Manage collision detection and response
- Maintain collection of physics components
- Provide global physics settings (gravity, etc.)

### 2.2 BasePhysicsComponent Class

```cpp
class BasePhysicsComponent : public IPhysicsComponent {
private:
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 acceleration;
    float mass;
    bool isStatic;
    
public:
    // Inherited from IPhysicsComponent
    void update(float deltaTime) override;
    void applyForce(const glm::vec3& force);
    void applyImpulse(const glm::vec3& impulse);
    
    // Getters and setters
    glm::vec3 getPosition() const;
    void setPosition(const glm::vec3& pos);
    float getMass() const;
    void setMass(float mass);
    glm::vec3 getVelocity() const;
    void setVelocity(const glm::vec3& vel);
};
```

**Key Features:**
- Newtonian physics simulation
- Force and impulse application
- Position/velocity/acceleration tracking
- Mass-based physical properties

### 2.3 SimpleCollisionSystem Class

```cpp
class SimpleCollisionSystem {
private:
    std::unique_ptr<BVH> bvh;
    std::vector<std::shared_ptr<CollisionEvent>> pendingEvents;
    
public:
    bool checkCollision(const IPhysicsComponent& a, const IPhysicsComponent& b);
    void updateObjects();
    void generateEvents();
    void processPendingEvents();
};
```

**Collision Detection Process:**
1. Query BVH for potentially colliding objects
2. Perform detailed AABB collision checks
3. Generate CollisionEvent for detected collisions
4. Queue events for processing

### 2.4 BVH (Bounding Volume Hierarchy)

```cpp
class BVH {
private:
    struct Node {
        BoundingVolume bounds;
        std::vector<std::shared_ptr<IPhysicsComponent>> objects;
        std::unique_ptr<Node> left;
        std::unique_ptr<Node> right;
    };
    
public:
    void insert(std::shared_ptr<IPhysicsComponent> object);
    void remove(std::shared_ptr<IPhysicsComponent> object);
    std::vector<std::shared_ptr<IPhysicsComponent>> query(const BoundingVolume& volume);
};
```

**Performance Characteristics:**
- O(log n) query time complexity
- Efficient spatial organization
- Dynamic updates for moving objects

### 2.5 EventSystem Class

```cpp
template<typename T>
class EventSystem {
private:
    std::unordered_map<std::type_index, std::vector<std::function<void(const T&)>>> subscribers;
    
public:
    void subscribe(std::function<void(const T&)> callback);
    void publish(const T& event);
    void clear();
};
```

## 3. Integration Architecture

### 3.1 GameObject/Actor Integration

```cpp
class GameObject {
protected:
    std::vector<std::shared_ptr<IPhysicsComponent>> physicsComponents;
    
public:
    virtual void update(float deltaTime);
    void addPhysicsComponent(std::shared_ptr<IPhysicsComponent> component);
    void removePhysicsComponent(std::shared_ptr<IPhysicsComponent> component);
};

class Actor : public GameObject {
private:
    Transform transform;
    
public:
    void update(float deltaTime) override;
    // Additional actor-specific functionality
};
```

### 3.2 Update Loop Integration

```cpp
void GameLoop::update(float deltaTime) {
    // Physics updates
    physicsEngine.update(deltaTime);
    
    // Event processing  
    eventSystem.processEvents();
    
    // Other game system updates...
}
```

## 4. Key Features and Capabilities

### 4.1 Physics Simulation

#### Newtonian Physics Model
- Mass-based force calculations
- Velocity integration using Euler method
- Acceleration from applied forces
- Gravity integration

#### Force Application
```cpp
void BasePhysicsComponent::applyForce(const glm::vec3& force) {
    acceleration += force / mass;
}
```

### 4.2 Collision Detection

#### AABB Collision Detection
```cpp
bool AABBOverlap(const BoundingVolume& a, const BoundingVolume& b) {
    return (a.min.x <= b.max.x && a.max.x >= b.min.x) &&
           (a.min.y <= b.max.y && a.max.y >= b.min.y) &&
           (a.min.z <= b.max.z && a.max.z >= b.min.z);
}
```

#### BVH Spatial Queries
```cpp
std::vector<std::shared_ptr<IPhysicsComponent>> 
BVH::query(const BoundingVolume& volume) {
    std::vector<std::shared_ptr<IPhysicsComponent>> results;
    queryRecursive(root, volume, results);
    return results;
}
```

### 4.3 Event System

#### Type-Safe Event Handling
```cpp
// Subscribe to collision events
eventSystem.subscribe<CollisionEvent>([](const CollisionEvent& event) {
    // Handle collision response
});

// Publish custom events
CollisionEvent collisionEvent(objectA, objectB);
eventSystem.publish(collisionEvent);
```

## 5. Performance Characteristics

### 5.1 Physics Engine Performance
- **Collision Detection**: O(log n) lookup time with BVH
- **Memory Usage**: Smart pointer management for efficient memory
- **Update Time**: Linear complexity with active components
- **Scalability**: Optimized for large numbers of objects

### 5.2 Event System Performance  
- **Processing Time**: Constant time event dispatching
- **Memory Efficiency**: Queue-based processing
- **Thread Safety**: Concurrent access support
- **Flexibility**: Easy addition of new event types

## 6. Usage Examples

### 6.1 Basic Physics Setup

```cpp
// Create player actor with physics
Actor* player = new Actor();
player->setPosition(glm::vec3(0, 0, 0));

// Add physics component
auto physics = std::make_shared<BasePhysicsComponent>();
physics->setMass(10.0f);
physics->setVelocity(glm::vec3(0, 5, 0));
player->addPhysicsComponent(physics);

// Set gravity
physicsEngine.setGravity(glm::vec3(0, -9.81, 0));
```

### 6.2 Event Subscription

```cpp
// Subscribe to collision events
eventSystem.subscribe<CollisionEvent>([](const CollisionEvent& event) {
    std::cout << "Collision between objects!" << std::endl;
    
    // Handle different types of collisions
    if (event.getColliderA()->getTag() == "Player") {
        // Player-specific response
    }
});

// Process events in update loop
void GameLoop::update(float deltaTime) {
    physicsEngine.update(deltaTime);
    eventSystem.processEvents();
}
```

### 6.3 Advanced Physics Usage

```cpp
// Apply forces to objects
physics->applyForce(glm::vec3(0, 10, 0)); // Apply upward force

// Handle object interactions
void handleObjectInteraction() {
    physicsEngine.update(deltaTime);
    
    // Process any generated events
    eventSystem.processEvents();
}
```

## 7. Testing and Validation

### 7.1 Component Testing
- Individual physics component functionality verification
- Collision detection algorithm validation
- Event system subscription/publishing tests
- Memory management and leak detection

### 7.2 Integration Testing
- Physics engine and event system integration
- GameObject/Actor physics component interaction
- Performance benchmarks for collision detection
- Multi-threaded access scenarios

## 8. Future Enhancements

### 8.1 Short-term Improvements
1. **Advanced Collision Shapes**
   - Sphere-based collision detection
   - Capsule support
   - Mesh-based geometry handling

2. **Enhanced Physics Features**  
   - Rigid body dynamics simulation
   - Joint and constraint systems
   - Advanced material properties

### 8.2 Medium-term Development
1. **Performance Optimizations**
   - Multi-threaded physics processing
   - Enhanced BVH algorithms
   - Memory pooling for frequent allocations

2. **Event System Improvements**
   - Event prioritization mechanisms
   - Asynchronous event handling
   - Better debugging and logging capabilities

### 8.3 Long-term Vision
1. **Advanced Physics Simulation**
   - Fluid dynamics support
   - Soft body physics
   - Particle system integration

2. **Enhanced Integration**
   - Network physics synchronization
   - AI interaction with physics
   - Advanced rendering integration

## 9. System Requirements

### 9.1 Dependencies
- C++17 standard compliance
- GLM mathematics library for vector operations
- Standard Template Library (STL) components
- Modern compiler support

### 9.2 Performance Requirements
- Physics updates at 60 FPS minimum
- Collision detection under 1ms per frame
- Event processing within frame budget
- Memory usage optimized for game environments

## 10. Conclusion

The physics engine and event system implementation provides a robust, scalable foundation for game development in the SGE framework. The modular design ensures extensibility while maintaining excellent performance characteristics. The integration with existing GameObject/Actor architecture enables developers to create realistic physical interactions with automatic collision detection and responsive event handling that supports modern game development requirements.

This specification serves as the complete technical reference for the implemented system, documenting all core components, interfaces, and functionality to support ongoing development and maintenance of the SGE physics and event systems.