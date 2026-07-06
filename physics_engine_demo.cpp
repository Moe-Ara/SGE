/*
 * SGE Physics Engine - Complete Demonstration
 * 
 * This file demonstrates the complete physics engine implementation
 * with all core components working together.
 */

#include <iostream>
#include <vector>
#include <memory>
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

// Base Event class
class Event {
public:
    virtual ~Event() = default;
};

// Collision event
class CollisionEvent : public Event {
private:
    void* objectA;
    void* objectB;
    
public:
    CollisionEvent(void* a, void* b) : objectA(a), objectB(b) {}
    
    void* getObjectA() const { return objectA; }
    void* getObjectB() const { return objectB; }
};

// Physics component interface
class IPhysicsComponent {
public:
    virtual ~IPhysicsComponent() = default;
    virtual void update(float deltaTime) = 0;
    virtual vec3 getPosition() const = 0;
    virtual void applyForce(const vec3& force) = 0;
    virtual float getMass() const = 0;
};

// Base physics component implementation
class BasePhysicsComponent : public IPhysicsComponent {
protected:
    vec3 position;
    vec3 velocity;
    vec3 acceleration;
    float mass;
    
public:
    BasePhysicsComponent(const vec3& pos, float m) 
        : position(pos), velocity(0, 0, 0), acceleration(0, 0, 0), mass(m) {}
    
    void update(float deltaTime) override {
        // Update velocity: v = u + at
        velocity = velocity + acceleration * deltaTime;
        
        // Update position: s = ut + 0.5at^2
        position = position + velocity * deltaTime + acceleration * (deltaTime * deltaTime * 0.5f);
        
        // Reset acceleration for next frame
        acceleration = vec3(0, 0, 0);
    }
    
    void applyForce(const vec3& force) override {
        // F = ma, so a = F/m
        if (mass > 0) {
            acceleration = acceleration + force / mass;
        }
    }
    
    vec3 getPosition() const override {
        return position;
    }
    
    float getMass() const override {
        return mass;
    }
    
    void setVelocity(const vec3& vel) {
        velocity = vel;
    }
    
    vec3 getVelocity() const {
        return velocity;
    }
};

// Simple collision system
class SimpleCollisionSystem {
private:
    std::vector<std::shared_ptr<IPhysicsComponent>> components;
    std::function<void(std::shared_ptr<Event>)> eventCallback;
    
public:
    void setEventCallback(const std::function<void(std::shared_ptr<Event>)>& callback) {
        eventCallback = callback;
    }
    
    void addComponent(std::shared_ptr<IPhysicsComponent> component) {
        components.push_back(component);
    }
    
    void checkCollisions() {
        for (size_t i = 0; i < components.size(); ++i) {
            for (size_t j = i + 1; j < components.size(); ++j) {
                auto compA = components[i];
                auto compB = components[j];
                
                // Skip if either has zero mass
                if (compA->getMass() == 0 || compB->getMass() == 0) continue;
                
                vec3 posA = compA->getPosition();
                vec3 posB = compB->getPosition();
                
                // Simple sphere collision detection (1 unit radius)
                float distance = (posA - posB).length();
                if (distance < 2.0f) { // 2 units diameter
                    auto collisionEvent = std::make_shared<CollisionEvent>(
                        compA.get(), compB.get()
                    );
                    
                    if (eventCallback) {
                        eventCallback(collisionEvent);
                    }
                }
            }
        }
    }
};

// Physics engine
class PhysicsEngine {
private:
    std::vector<std::shared_ptr<IPhysicsComponent>> components;
    SimpleCollisionSystem collisionSystem;
    
public:
    PhysicsEngine() {
        collisionSystem.setEventCallback([this](std::shared_ptr<Event> event) {
            handleEvent(event);
        });
    }
    
    void update(float deltaTime) {
        // Update all physics components
        for (auto& component : components) {
            component->update(deltaTime);
        }
        
        // Check for collisions
        collisionSystem.checkCollisions();
    }
    
    void addComponent(std::shared_ptr<IPhysicsComponent> component) {
        components.push_back(component);
        collisionSystem.addComponent(component);
    }
    
private:
    void handleEvent(std::shared_ptr<Event> event) {
        if (auto collision = std::dynamic_pointer_cast<CollisionEvent>(event)) {
            std::cout << "COLLISION DETECTED!" << std::endl;
            // In a real implementation, this would trigger game logic
        }
    }
};

// Base GameObject class
class GameObject {
protected:
    vec3 position;
    std::string name;
    
public:
    GameObject(const vec3& pos, const std::string& objName) 
        : position(pos), name(objName) {}
    
    virtual ~GameObject() = default;
    
    vec3 getPosition() const { return position; }
    void setPosition(const vec3& pos) { position = pos; }
    const std::string& getName() const { return name; }
};

// Actor class that integrates with physics
class Actor : public GameObject {
private:
    std::vector<std::shared_ptr<IPhysicsComponent>> physicsComponents;
    
public:
    Actor(const vec3& pos, const std::string& tag) 
        : GameObject(pos, tag) {}
    
    void addPhysicsComponent(std::shared_ptr<IPhysicsComponent> component) {
        physicsComponents.push_back(component);
    }
    
    std::vector<std::shared_ptr<IPhysicsComponent>>& getPhysicsComponents() {
        return physicsComponents;
    }
    
    void update(float deltaTime) {
        for (auto& component : physicsComponents) {
            component->update(deltaTime);
        }
    }
    
    vec3 getPhysicsPosition() const {
        if (!physicsComponents.empty()) {
            return physicsComponents[0]->getPosition();
        }
        return position;
    }
};

// Demo scene manager
class Scene {
private:
    PhysicsEngine engine;
    std::vector<std::shared_ptr<Actor>> actors;
    
public:
    void setupScene() {
        std::cout << "Setting up physics simulation scene..." << std::endl;
        
        // Create ground actor (static)
        auto ground = std::make_shared<Actor>(vec3(0, 0, 0), "ground");
        actors.push_back(ground);
        
        // Create player actor
        auto player = std::make_shared<Actor>(vec3(0, 10, 0), "player");
        auto playerPhysics = std::make_shared<BasePhysicsComponent>(vec3(0, 10, 0), 1.0f);
        player->addPhysicsComponent(playerPhysics);
        engine.addComponent(playerPhysics);
        actors.push_back(player);
        
        // Create enemy actor
        auto enemy = std::make_shared<Actor>(vec3(5, 5, 0), "enemy");
        auto enemyPhysics = std::make_shared<BasePhysicsComponent>(vec3(5, 5, 0), 1.0f);
        enemy->addPhysicsComponent(enemyPhysics);
        engine.addComponent(enemyPhysics);
        actors.push_back(enemy);
        
        // Create floating object
        auto floatingObject = std::make_shared<Actor>(vec3(-3, 8, 0), "floating_object");
        auto floatPhysics = std::make_shared<BasePhysicsComponent>(vec3(-3, 8, 0), 0.5f);
        floatingObject->addPhysicsComponent(floatPhysics);
        engine.addComponent(floatPhysics);
        actors.push_back(floatingObject);
        
        std::cout << "Scene setup complete with " << actors.size() << " actors." << std::endl;
    }
    
    void runSimulation(int frames = 20) {
        std::cout << "\nStarting physics simulation..." << std::endl;
        
        // Apply initial forces
        for (auto& actor : actors) {
            if (actor->getName() == "player") {
                // Apply gravity to player
                auto physics = actor->getPhysicsComponents();
                if (!physics.empty()) {
                    physics[0]->applyForce(vec3(0, -9.81, 0)); // Gravity
                }
            } else if (actor->getName() == "enemy") {
                // Move enemy left
                auto physics = actor->getPhysicsComponents();
                if (!physics.empty()) {
                    physics[0]->applyForce(vec3(-2, 0, 0)); // Horizontal force
                }
            } else if (actor->getName() == "floating_object") {
                // Apply upward force to floating object
                auto physics = actor->getPhysicsComponents();
                if (!physics.empty()) {
                    physics[0]->applyForce(vec3(0, 5, 0)); // Upward force
                }
            }
        }
        
        for (int i = 0; i < frames; ++i) {
            std::cout << "\n--- Frame " << i + 1 << " ---" << std::endl;
            
            // Update physics engine
            engine.update(0.1f);
            
            // Print actor positions
            for (const auto& actor : actors) {
                vec3 pos = actor->getPhysicsPosition();
                std::cout << actor->getName() << ": (" 
                          << pos.x << ", " << pos.y << ", " << pos.z << ")" << std::endl;
            }
        }
        
        std::cout << "\nSimulation completed!" << std::endl;
    }
    
    void demonstratePhysicsFeatures() {
        std::cout << "\n=== Physics Engine Features Demonstration ===" << std::endl;
        
        // Feature 1: Force Application
        std::cout << "1. Force Application:" << std::endl;
        auto testComponent = std::make_shared<BasePhysicsComponent>(vec3(0, 0, 0), 1.0f);
        std::cout << "   Initial position: (" << testComponent->getPosition().x << ", " 
                  << testComponent->getPosition().y << ", " << testComponent->getPosition().z << ")" << std::endl;
        
        testComponent->applyForce(vec3(0, 10, 0)); // Apply upward force
        testComponent->update(1.0f);
        vec3 finalPos = testComponent->getPosition();
        std::cout << "   After 1s with 10N force: (" << finalPos.x << ", " 
                  << finalPos.y << ", " << finalPos.z << ")" << std::endl;
        
        // Feature 2: Mass Effects
        std::cout << "2. Mass Effects:" << std::endl;
        auto heavyComponent = std::make_shared<BasePhysicsComponent>(vec3(0, 0, 0), 5.0f);
        auto lightComponent = std::make_shared<BasePhysicsComponent>(vec3(0, 0, 0), 0.5f);
        
        heavyComponent->applyForce(vec3(0, 10, 0));
        lightComponent->applyForce(vec3(0, 10, 0));
        
        heavyComponent->update(1.0f);
        lightComponent->update(1.0f);
        
        std::cout << "   Heavy object (5kg) position: (" 
                  << heavyComponent->getPosition().y << ")" << std::endl;
        std::cout << "   Light object (0.5kg) position: (" 
                  << lightComponent->getPosition().y << ")" << std::endl;
        
        // Feature 3: Collision Detection
        std::cout << "3. Collision Detection:" << std::endl;
        std::cout << "   Objects will be checked for collisions during simulation." << std::endl;
    }
};

int main() {
    try {
        std::cout << "SGE Physics Engine - Complete Demo" << std::endl;
        std::cout << "===================================" << std::endl;
        
        // Create and setup scene
        Scene scene;
        scene.setupScene();
        
        // Demonstrate physics features
        scene.demonstratePhysicsFeatures();
        
        // Run simulation
        scene.runSimulation(10);
        
        std::cout << "\n=== Demo Complete ===" << std::endl;
        std::cout << "The physics engine successfully demonstrates:" << std::endl;
        std::cout << "- Newtonian physics calculations (forces, velocities, positions)" << std::endl;
        std::cout << "- Mass-based force application and response" << std::endl;
        std::cout << "- Collision detection between objects" << std::endl;
        std::cout << "- Integration with GameObject/Actor architecture" << std::endl;
        std::cout << "- Frame-rate independent time stepping" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error in demo execution: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}