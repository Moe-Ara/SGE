# Physics and Event System Implementation

This project now includes a basic physics engine with an event system for communication between components.

## Key Components

### Physics Engine
- `PhysicsEngine`: Main physics simulation manager
- `BasePhysicsComponent`: Base class for physics properties
- `SimpleCollisionSystem`: Collision detection using BVH (Bounding Volume Hierarchy)
- `BVH`: Bounding Volume Hierarchy for efficient collision detection

### Event System  
- `EventSystem`: Central event management system
- `IEventSystem`: Interface for event handling
- `CollisionEvent`: Specific event type for collision notifications

## Features Implemented

1. **Physics Simulation**
   - Gravity and force application
   - Position integration
   - Collision detection using BVH

2. **Event System**
   - Publish/subscribe pattern
   - Collision events
   - Extensible event architecture
   - Thread-safe event handling

3. **Integration**
   - Physics engine with event system support
   - Actor-based game object system
   - Modular design for easy extension

## Usage Example

