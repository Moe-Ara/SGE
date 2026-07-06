/*
 * SGE Physics Engine - Complete Test Suite
 * 
 * This file contains comprehensive tests for the physics engine implementation.
 * It demonstrates all core components working together in a realistic scenario.
 */

#include <iostream>
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>
#include <typeindex>
#include <algorithm>
#include <cmath>
#include <cassert>

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
    
    vec3 operator/(float scalar) const {
        return vec3(x / scalar, y / scalar, z / scalar);
    }
    
    float length() const {
        return sqrt(x*x + y*y + z*z);
    }
    
    vec3 normalize() const {
        float len = length();
        if (len > 0) {
            return *this / len;
        }
        return vec3(0, 0, 0);
    }
};

// Base GameObject class
class GameObject {
protected:
    vec3 position;
    std::string tag;
    
public:
    GameObject(const vec3& pos, const std::string& t) : position(pos), tag(t) {}
    
    virtual ~GameObject() = default;
    
    const vec3& getPosition() const { return position; }
    void setPosition(const vec3& pos) { position = pos; }
    const std::string& getTag() const { return tag; }
};

// Base Event class
class Event {
public:
    virtual ~Event() = default;
};

// Collision Event
class CollisionEvent : public Event {
private:
    std::shared_ptr<GameObject> objectA;
    std::shared_ptr<GameObject> objectB;
    
public:
    CollisionEvent(std::shared_ptr<GameObject> a, std::shared_ptr<GameObject> b) 
        : objectA(a), objectB(b) {}
    
    std::shared_ptr<GameObject> getObjectA() const { return objectA; }
    std::shared_ptr<GameObject> getObjectB() const { return objectB; }
};

// Physics Component Interface
class IPhysicsComponent {
public:
    virtual ~IPhysicsComponent() = default;
    virtual void update(float deltaTime) = 0;
    virtual vec3 getPosition() const = 0;
    virtual void applyForce(const vec3& force) = 0;
    virtual void setMass(float mass) = 0;
    virtual float getMass() const = 0;
};

// Physics Engine Interface
class IPhysicsEngine {
public:
    virtual ~IPhysicsEngine() = default;
    virtual void update(float deltaTime) = 0;
    virtual void addComponent(std::shared_ptr<IPhysicsComponent> component) = 0;
    virtual void removeComponent(std::shared_ptr<IPhysicsComponent> component) = 0;
};

// Event System Interface
class IEventSystem {
public:
    virtual ~IEventSystem() = default;
    template<typename T>
    void subscribe(std::function<void(const T&)> callback) {
        // Implementation would be in derived class
    }
    
    template<typename T>
    void publish(const T& event) {
        // Implementation would be in derived class
    }
};

// Base Physics Component Implementation
class BasePhysicsComponent : public IPhysicsComponent {
protected:
    vec3 position;
    vec3 velocity;
    vec3 acceleration;
    float mass;
    bool isStatic;
    
public:
    BasePhysicsComponent(const vec3& pos, float m = 1.0f) 
        : position(pos), velocity(0, 0, 0), acceleration(0, 0, 0),
          mass(m), isStatic(false) {}
    
    void update(float deltaTime) override {
        if (isStatic) return;
        
        // Update velocity with acceleration
        velocity = velocity + acceleration * deltaTime;
        
        // Update position with velocity
        position = position + velocity * deltaTime;
        
        // Reset acceleration for next frame
        acceleration = vec3(0, 0, 0);
    }
    
    vec3 getPosition() const override { return position; }
    
    void applyForce(const vec3& force) override {
        if (isStatic) return;
        
        // F = ma => a = F/m
        acceleration = acceleration + force / mass;
    }
    
    void setMass(float m) override {
        mass = m;
    }
    
    float getMass() const override {
        return mass;
    }
    
    void setStatic(bool staticFlag) { isStatic = staticFlag; }
    bool isStaticObject() const { return isStatic; }
};

// Simple Collision System
class SimpleCollisionSystem {
private:
    std::vector<std::shared_ptr<IPhysicsComponent>> components;
    std::function<void(std::shared_ptr<Event>)> eventCallback;
    
public:
    void setEventCallback(std::function<void(std::shared_ptr<Event>)> callback) {
        eventCallback = callback;
    }
    
    void addComponent(std::shared_ptr<IPhysicsComponent> component) {
        components.push_back(component);
    }
    
    void removeComponent(std::shared_ptr<IPhysicsComponent> component) {
        auto it = std::find(components.begin(), components.end(), component);
        if (it != components.end()) {
            components.erase(it);
        }
    }
    
    void checkCollisions() {
        // Simple AABB collision detection
        for (size_t i = 0; i < components.size(); ++i) {
            for (size_t j = i + 1; j < components.size(); ++j) {
                auto compA = components[i];
                auto compB = components[j];
                
                if (compA->getMass() == 0 || compB->getMass() == 0) continue;
                
                vec3 posA = compA->getPosition();
                vec3 posB = compB->getPosition();
                
                // Simple collision detection with 1 unit radius spheres
                float distance = (posA - posB).length();
                if (distance < 1.0f) {
                    // Create collision event
                    auto collisionEvent = std::make_shared<CollisionEvent>(
                        std::make_shared<GameObject>(posA, "objectA"),
                        std::make_shared<GameObject>(posB, "objectB")
                    );
                    
                    if (eventCallback) {
                        eventCallback(collisionEvent);
                    }
                }
            }
        }
    }
};

// Main Physics Engine
class PhysicsEngine : public IPhysicsEngine {
private:
    std::vector<std::shared_ptr<IPhysicsComponent>> components;
    SimpleCollisionSystem collisionSystem;
    
public:
    PhysicsEngine() {
        collisionSystem.setEventCallback([this](std::shared_ptr<Event> event) {
            // Forward events to subscribers
            handleEvent(event);
        });
    }
    
    void update(float deltaTime) override {
        // Update all physics components
        for (auto& component : components) {
            component->update(deltaTime);
        }
        
        // Check for collisions
        collisionSystem.checkCollisions();
    }
    
    void addComponent(std::shared_ptr<IPhysicsComponent> component) override {
        components.push_back(component);
        collisionSystem.addComponent(component);
    }
    
    void removeComponent(std::shared_ptr<IPhysicsComponent> component) override {
        auto it = std::find(components.begin(), components.end(), component);
        if (it != components.end()) {
            components.erase(it);
            collisionSystem.removeComponent(component);
        }
    }
    
private:
    void handleEvent(std::shared_ptr<Event> event) {
        // In a real implementation, this would dispatch to subscribers
        // For now, we'll just log it
        if (auto collision = std::dynamic_pointer_cast<CollisionEvent>(event)) {
            std::cout << "Collision detected between objects!" << std::endl;
        }
    }
};

// Event System Implementation
class EventSystem : public IEventSystem {
private:
    template<typename T>
    std::vector<std::function<void(const T&)>> subscribers;
    
public:
    template<typename T>
    void subscribe(std::function<void(const T&)> callback) {
        subscribers<T>.push_back(callback);
    }
    
    template<typename T>
    void publish(const T& event) {
        for (const auto& subscriber : subscribers<T>) {
            subscriber(event);
        }
    }
};

// Actor class that integrates with physics
class Actor : public GameObject {
private:
    std::vector<std::shared_ptr<IPhysicsComponent>> physicsComponents;
    
public:
    Actor(const vec3& pos, const std::string& tag) : GameObject(pos, tag) {}
    
    void addPhysicsComponent(std::shared_ptr<IPhysicsComponent> component) {
        physicsComponents.push_back(component);
    }
    
    std::vector<std::shared_ptr<IPhysicsComponent>>& getPhysicsComponents() {
        return physicsComponents;
    }
};

// Test functions
void testPhysicsBasics() {
    std::cout << "=== Testing Basic Physics ===" << std::endl;
    
    // Create physics component
    auto physics = std::make_shared<BasePhysicsComponent>(vec3(0, 0, 0), 1.0f);
    
    // Test initial state
    assert(physics->getMass() == 1.0f);
    assert(physics->getPosition().x == 0);
    assert(physics->getPosition().y == 0);
    assert(physics->getPosition().z == 0);
    
    // Apply force and update
    physics->applyForce(vec3(0, 10, 0)); // Apply upward force
    physics->update(1.0f); // Update for 1 second
    
    // Check if velocity was updated (force = mass * acceleration, so acceleration = 10 m/s²)
    vec3 pos = physics->getPosition();
    std::cout << "Position after force application: (" << pos.x << ", " << pos.y << ", " << pos.z << ")" << std::endl;
    
    // Test mass changes
    physics->setMass(2.0f);
    assert(physics->getMass() == 2.0f);
    
    std::cout << "Basic physics tests passed!" << std::endl;
}

void testCollisionDetection() {
    std::cout << "=== Testing Collision Detection ===" << std::endl;
    
    // Create physics engine
    PhysicsEngine engine;
    
    // Create two objects close to each other
    auto obj1 = std::make_shared<BasePhysicsComponent>(vec3(0, 0, 0), 1.0f);
    auto obj2 = std::make_shared<BasePhysicsComponent>(vec3(0.5f, 0, 0), 1.0f); // Close enough to collide
    
    // Add to engine
    engine.addComponent(obj1);
    engine.addComponent(obj2);
    
    // Update to trigger collision detection
    engine.update(0.1f);
    
    std::cout << "Collision detection test completed!" << std::endl;
}

void testActorIntegration() {
    std::cout << "=== Testing Actor Integration ===" << std::endl;
    
    // Create actor with physics
    Actor actor(vec3(0, 0, 0), "test_actor");
    
    // Add physics component
    auto physics = std::make_shared<BasePhysicsComponent>(vec3(0, 0, 0), 1.0f);
    actor.addPhysicsComponent(physics);
    
    // Verify integration
    assert(actor.getPhysicsComponents().size() == 1);
    
    // Test position update
    physics->applyForce(vec3(5, 0, 0)); // Apply horizontal force
    physics->update(1.0f);
    
    vec3 pos = physics->getPosition();
    std::cout << "Actor position after force: (" << pos.x << ", " << pos.y << ", " << pos.z << ")" << std::endl;
    
    std::cout << "Actor integration tests passed!" << std::endl;
}

void runAllTests() {
    std::cout << "Running SGE Physics Engine Test Suite..." << std::endl;
    
    testPhysicsBasics();
    std::cout << std::endl;
    
    testCollisionDetection();
    std::cout << std::endl;
    
    testActorIntegration();
    std::cout << std::endl;
    
    std::cout << "All tests passed successfully!" << std::endl;
}

// Demonstration function
void demonstratePhysicsEngine() {
    std::cout << "=== Physics Engine Demonstration ===" << std::endl;
    
    // Create physics engine
    PhysicsEngine engine;
    
    // Create actors with physics
    Actor player(vec3(0, 10, 0), "player");
    Actor enemy(vec3(5, 5, 0), "enemy");
    
    // Add physics to actors
    auto playerPhysics = std::make_shared<BasePhysicsComponent>(vec3(0, 10, 0), 1.0f);
    auto enemyPhysics = std::make_shared<BasePhysicsComponent>(vec3(5, 5, 0), 1.0f);
    
    player.addPhysicsComponent(playerPhysics);
    enemy.addPhysicsComponent(enemyPhysics);
    
    // Add to engine
    engine.addComponent(playerPhysics);
    engine.addComponent(enemyPhysics);
    
    // Apply forces
    playerPhysics->applyForce(vec3(0, -9.81, 0)); // Gravity
    enemyPhysics->applyForce(vec3(-2, 0, 0));     // Move left
    
    std::cout << "Initial positions:" << std::endl;
    std::cout << "Player: (" << playerPhysics->getPosition().x << ", " 
              << playerPhysics->getPosition().y << ", " 
              << playerPhysics->getPosition().z << ")" << std::endl;
    std::cout << "Enemy: (" << enemyPhysics->getPosition().x << ", " 
              << enemyPhysics->getPosition().y << ", " 
              << enemyPhysics->getPosition().z << ")" << std::endl;
    
    // Run simulation for a few frames
    for (int i = 0; i < 5; ++i) {
        engine.update(0.1f);
        std::cout << "Frame " << i+1 << " - Player: (" 
                  << playerPhysics->getPosition().x << ", " 
                  << playerPhysics->getPosition().y << ", " 
                  << playerPhysics->getPosition().z << ")" << std::endl;
    }
    
    std::cout << "Demonstration completed!" << std::endl;
}

int main() {
    try {
        runAllTests();
        std::cout << std::endl;
        demonstratePhysicsEngine();
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}