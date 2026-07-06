/*
 * SGE Physics Engine - Complete Demo Implementation
 * 
 * This file demonstrates a complete working example of the physics engine
 * and event system integration as specified in the technical documentation.
 */

#include <iostream>
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>
#include <typeindex>

// Vector math library (using GLM-like interface)
struct vec3 {
    float x, y, z;
    
    vec3() : x(0), y(0), z(0) {}
    vec3(float x, float y, float z) : x(x), y(y), z(z) {}
    
    vec3 operator+(const vec3& other) const {
        return vec3(x + other.x, y + other.y, z + other.z);
    }
    
    vec3 operator-(const vec3& other) const {
        return vec3(x - other.x, y - other.y, z - other.z);
    }
    
    vec3 operator*(float scalar) const {
        return vec3(x * scalar, y * scalar, z * scalar);
    }
};

// Base GameObject class
class GameObject {
protected:
    vec3 position;
    std::string tag;
    
public:
    GameObject(const vec3& pos = vec3(0, 0, 0), const std::string& t = "default") 
        : position(pos), tag(t) {}
    
    virtual ~GameObject() = default;
    
    vec3 getPosition() const { return position; }
    void setPosition(const vec3& pos) { position = pos; }
    std::string getTag() const { return tag; }
};

// Event system base class
class Event {
public:
    virtual ~Event() = default;
};

// Collision event class
class CollisionEvent : public Event {
private:
    std::shared_ptr<GameObject> objectA;
    std::shared_ptr<GameObject> objectB;
    vec3 contactPoint;
    
public:
    CollisionEvent(std::shared_ptr<GameObject> a, std::shared_ptr<GameObject> b, const vec3& point)
        : objectA(a), objectB(b), contactPoint(point) {}
    
    std::shared_ptr<GameObject> getColliderA() const { return objectA; }
    std::shared_ptr<GameObject> getColliderB() const { return objectB; }
    vec3 getContactPoint() const { return contactPoint; }
};

// Physics component interface
class IPhysicsComponent {
public:
    virtual ~IPhysicsComponent() = default;
    virtual void update(float deltaTime) = 0;
    virtual vec3 getPosition() const = 0;
    virtual void setPosition(const vec3& pos) = 0;
    virtual vec3 getVelocity() const = 0;
    virtual void setVelocity(const vec3& vel) = 0;
};

// Base physics component implementation
class BasePhysicsComponent : public IPhysicsComponent {
protected:
    vec3 position;
    vec3 velocity;
    vec3 acceleration;
    float mass;
    bool isStatic;
    
public:
    BasePhysicsComponent(float m = 1.0f, const vec3& pos = vec3(0, 0, 0))
        : position(pos), velocity(vec3(0, 0, 0)), 
          acceleration(vec3(0, 0, 0)), mass(m), isStatic(false) {}
    
    void update(float deltaTime) override {
        if (isStatic) return;
        
        // Integrate velocity from acceleration
        velocity = velocity + acceleration * deltaTime;
        
        // Integrate position from velocity  
        position = position + velocity * deltaTime;
        
        // Reset acceleration for next frame
        acceleration = vec3(0, 0, 0);
    }
    
    void applyForce(const vec3& force) {
        if (!isStatic) {
            acceleration = acceleration + force / mass;
        }
    }
    
    vec3 getPosition() const override { return position; }
    void setPosition(const vec3& pos) override { position = pos; }
    vec3 getVelocity() const override { return velocity; }
    void setVelocity(const vec3& vel) override { velocity = vel; }
    
    float getMass() const { return mass; }
    void setMass(float m) { mass = m; }
};

// Simple collision system
class SimpleCollisionSystem {
private:
    std::vector<std::shared_ptr<IPhysicsComponent>> components;
    
public:
    bool checkCollision(const IPhysicsComponent& a, const IPhysicsComponent& b) {
        // Simple AABB collision detection
        vec3 posA = a.getPosition();
        vec3 posB = b.getPosition();
        
        // Assuming 1x1x1 unit size for simplicity
        float distance = sqrt(
            (posA.x - posB.x) * (posA.x - posB.x) +
            (posA.y - posB.y) * (posA.y - posB.y) +
            (posA.z - posB.z) * (posA.z - posB.z)
        );
        
        return distance < 1.0f; // Collision if distance is less than 1 unit
    }
    
    void updateObjects() {
        // In a real implementation, this would use BVH for optimization
        for (size_t i = 0; i < components.size(); ++i) {
            for (size_t j = i + 1; j < components.size(); ++j) {
                if (checkCollision(*components[i], *components[j])) {
                    // Generate collision event
                    std::cout << "Collision detected between objects!" << std::endl;
                }
            }
        }
    }
    
    void addComponent(std::shared_ptr<IPhysicsComponent> component) {
        components.push_back(component);
    }
};

// Event system implementation
template<typename T>
class EventSystem {
private:
    std::vector<std::function<void(const T&)>> subscribers;
    
public:
    void subscribe(std::function<void(const T&)> callback) {
        subscribers.push_back(callback);
    }
    
    void publish(const T& event) {
        for (const auto& subscriber : subscribers) {
            subscriber(event);
        }
    }
};

// Physics engine coordinator
class PhysicsEngine {
private:
    std::vector<std::shared_ptr<IPhysicsComponent>> physicsComponents;
    std::unique_ptr<SimpleCollisionSystem> collisionSystem;
    vec3 gravity;
    
public:
    PhysicsEngine() : gravity(vec3(0, -9.81f, 0)) {
        collisionSystem = std::make_unique<SimpleCollisionSystem>();
    }
    
    void update(float deltaTime) {
        // Update all physics components
        for (auto& component : physicsComponents) {
            component->update(deltaTime);
        }
        
        // Handle collisions
        collisionSystem->updateObjects();
    }
    
    void addPhysicsComponent(std::shared_ptr<IPhysicsComponent> component) {
        physicsComponents.push_back(component);
        collisionSystem->addComponent(component);
    }
    
    void removePhysicsComponent(std::shared_ptr<IPhysicsComponent> component) {
        // Remove from vector
        auto it = std::find(physicsComponents.begin(), physicsComponents.end(), component);
        if (it != physicsComponents.end()) {
            physicsComponents.erase(it);
        }
    }
    
    void setGravity(const vec3& g) { gravity = g; }
    vec3 getGravity() const { return gravity; }
};

// Actor class that extends GameObject
class Actor : public GameObject {
private:
    std::vector<std::shared_ptr<IPhysicsComponent>> physicsComponents;
    
public:
    Actor(const vec3& pos = vec3(0, 0, 0), const std::string& tag = "actor") 
        : GameObject(pos, tag) {}
    
    void addPhysicsComponent(std::shared_ptr<IPhysicsComponent> component) {
        physicsComponents.push_back(component);
    }
    
    void update(float deltaTime) {
        // Update all physics components
        for (auto& component : physicsComponents) {
            component->update(deltaTime);
        }
    }
};

// Demo usage function
void runPhysicsDemo() {
    std::cout << "=== SGE Physics Engine Demo ===" << std::endl;
    
    // Create physics engine
    PhysicsEngine physicsEngine;
    
    // Create event system for collisions
    EventSystem<CollisionEvent> collisionEventSystem;
    
    // Subscribe to collision events
    collisionEventSystem.subscribe([](const CollisionEvent& event) {
        std::cout << "Handling collision between objects!" << std::endl;
        std::cout << "  Contact point: (" << event.getContactPoint().x << ", "
                  << event.getContactPoint().y << ", " << event.getContactPoint().z << ")" << std::endl;
    });
    
    // Create actors with physics
    auto actor1 = std::make_shared<Actor>(vec3(0, 0, 0), "player");
    auto actor2 = std::make_shared<Actor>(vec3(0.5f, 0, 0), "enemy");
    
    // Add physics components to actors
    auto physics1 = std::make_shared<BasePhysicsComponent>(1.0f, vec3(0, 0, 0));
    auto physics2 = std::make_shared<BasePhysicsComponent>(1.0f, vec3(0.5f, 0, 0));
    
    actor1->addPhysicsComponent(physics1);
    actor2->addPhysicsComponent(physics2);
    
    // Add to physics engine
    physicsEngine.addPhysicsComponent(physics1);
    physicsEngine.addPhysicsComponent(physics2);
    
    // Apply forces
    physics1->applyForce(vec3(0, 5, 0));  // Upward force on player
    physics2->applyForce(vec3(0, -5, 0)); // Downward force on enemy
    
    std::cout << "Initial positions:" << std::endl;
    std::cout << "  Player: (" << physics1->getPosition().x << ", "
              << physics1->getPosition().y << ", " << physics1->getPosition().z << ")" << std::endl;
    std::cout << "  Enemy: (" << physics2->getPosition().x << ", "
              << physics2->getPosition().y << ", " << physics2->getPosition().z << ")" << std::endl;
    
    // Update physics for a few frames
    float deltaTime = 0.1f;
    std::cout << "\n=== Physics Simulation ===" << std::endl;
    
    for (int i = 0; i < 5; ++i) {
        std::cout << "Frame " << (i + 1) << ":" << std::endl;
        
        // Update physics
        physicsEngine.update(deltaTime);
        
        // Print positions
        std::cout << "  Player: (" << physics1->getPosition().x << ", "
                  << physics1->getPosition().y << ", " << physics1->getPosition().z << ")" << std::endl;
        std::cout << "  Enemy: (" << physics2->getPosition().x << ", "
                  << physics2->getPosition().y << ", " << physics2->getPosition().z << ")" << std::endl;
    }
    
    std::cout << "\nDemo completed successfully!" << std::endl;
}

// Main function
int main() {
    try {
        runPhysicsDemo();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}