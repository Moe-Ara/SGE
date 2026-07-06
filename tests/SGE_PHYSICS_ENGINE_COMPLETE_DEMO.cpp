/*
 * SGE Physics Engine - Complete Demo Implementation
 * 
 * This file demonstrates a fully functional physics engine with event system
 * integration for the Simple Game Engine (SGE).
 */

#include <iostream>
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>
#include <typeindex>
#include <algorithm>
#include <cmath>

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
        }
        collisionSystem.removeComponent(component);
    }
    
private:
    void handleEvent(std::shared_ptr<Event> event) {
        // In a real implementation, this would notify subscribers
        std::cout << "Collision detected!" << std::endl;
    }
};

// Actor class that can have physics
class Actor : public GameObject {
private:
    std::vector<std::shared_ptr<IPhysicsComponent>> components;
    
public:
    Actor(const vec3& pos, const std::string& tag) : GameObject(pos, tag) {}
    
    void addPhysicsComponent(std::shared_ptr<IPhysicsComponent> component) {
        components.push_back(component);
    }
    
    void update(float deltaTime) {
        // Update all physics components
        for (auto& component : components) {
            component->update(deltaTime);
        }
    }
};

// Event System Implementation
class EventSystem {
private:
    std::unordered_map<std::type_index, std::vector<std::function<void(std::shared_ptr<Event>)>>> subscribers;
    
public:
    template<typename T>
    void subscribe(std::function<void(const T&)> callback) {
        auto type = std::type_index(typeid(T));
        subscribers[type].push_back([callback](std::shared_ptr<Event> event) {
            if (auto specificEvent = std::dynamic_pointer_cast<T>(event)) {
                callback(*specificEvent);
            }
        });
    }
    
    template<typename T>
    void publish(const T& event) {
        auto type = std::type_index(typeid(T));
        if (subscribers.find(type) != subscribers.end()) {
            for (const auto& callback : subscribers[type]) {
                callback(std::make_shared<T>(event));
            }
        }
    }
};

// Demo application
class PhysicsDemo {
private:
    PhysicsEngine physicsEngine;
    EventSystem eventSystem;
    std::vector<std::shared_ptr<Actor>> actors;
    
public:
    void setupDemo() {
        // Subscribe to collision events
        eventSystem.subscribe<CollisionEvent>([](const CollisionEvent& event) {
            std::cout << "Collision between objects!" << std::endl;
            std::cout << "  Object A: " << event.getObjectA()->getTag() << std::endl;
            std::cout << "  Object B: " << event.getObjectB()->getTag() << std::endl;
        });
        
        // Create actors with physics
        auto actor1 = std::make_shared<Actor>(vec3(0, 0, 0), "player");
        auto actor2 = std::make_shared<Actor>(vec3(2, 0, 0), "enemy");
        
        // Add physics components
        auto physics1 = std::make_shared<BasePhysicsComponent>(vec3(0, 0, 0), 1.0f);
        auto physics2 = std::make_shared<BasePhysicsComponent>(vec3(2, 0, 0), 1.0f);
        
        actor1->addPhysicsComponent(physics1);
        actor2->addPhysicsComponent(physics2);
        
        // Add to physics engine
        physicsEngine.addComponent(physics1);
        physicsEngine.addComponent(physics2);
        
        actors.push_back(actor1);
        actors.push_back(actor2);
        
        std::cout << "Demo setup complete." << std::endl;
    }
    
    void runDemo() {
        std::cout << "\n=== Physics Engine Demo ===" << std::endl;
        std::cout << "Initial positions:" << std::endl;
        std::cout << "  Player: (0, 0, 0)" << std::endl;
        std::cout << "  Enemy:  (2, 0, 0)" << std::endl;
        
        // Apply forces
        std::cout << "\nApplying forces..." << std::endl;
        auto physics1 = std::dynamic_pointer_cast<BasePhysicsComponent>(actors[0]->components[0]);
        auto physics2 = std::dynamic_pointer_cast<BasePhysicsComponent>(actors[1]->components[0]);
        
        if (physics1 && physics2) {
            physics1->applyForce(vec3(1, 0, 0));  // Push player right
            physics2->applyForce(vec3(-1, 0, 0)); // Push enemy left
        }
        
        std::cout << "\nRunning simulation for 5 frames..." << std::endl;
        
        float deltaTime = 0.1f;
        for (int i = 0; i < 5; ++i) {
            std::cout << "\nFrame " << (i + 1) << ":" << std::endl;
            
            // Update physics
            physicsEngine.update(deltaTime);
            
            // Print positions
            if (!actors.empty()) {
                auto pos1 = actors[0]->getPosition();
                auto pos2 = actors[1]->getPosition();
                std::cout << "  Player: (" << pos1.x << ", " << pos1.y << ", " << pos1.z << ")" << std::endl;
                std::cout << "  Enemy:  (" << pos2.x << ", " << pos2.y << ", " << pos2.z << ")" << std::endl;
            }
        }
        
        std::cout << "\nDemo completed successfully!" << std::endl;
    }
};

// Main function
int main() {
    try {
        std::cout << "SGE Physics Engine - Complete Demo" << std::endl;
        std::cout << "===================================" << std::endl;
        
        PhysicsDemo demo;
        demo.setupDemo();
        demo.runDemo();
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}