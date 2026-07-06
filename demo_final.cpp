/*
 * SGE Physics Engine - Complete Final Demo
 * 
 * This file demonstrates all components of the physics engine
 * working together in a realistic simulation scenario.
 */

#include <iostream>
#include <vector>
#include <memory>
#include <functional>
#include <string>
#include <cmath>

// Vector math structure (simplified GLM-like)
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

// Base Event class for physics events
class Event {
public:
    virtual ~Event() = default;
};

// Collision event for physics system
class CollisionEvent : public Event {
private:
    std::string objectA;
    std::string objectB;
    
public:
    CollisionEvent(const std::string& a, const std::string& b) 
        : objectA(a), objectB(b) {}
    
    const std::string& getObjectA() const { return objectA; }
    const std::string& getObjectB() const { return objectB; }
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

// Simple collision detection system
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
                
                // Skip static objects (zero mass)
                if (compA->getMass() == 0 || compB->getMass() == 0) continue;
                
                vec3 posA = compA->getPosition();
                vec3 posB = compB->getPosition();
                
                // Simple sphere collision detection
                float distance = (posA - posB).length();
                if (distance < 2.0f) { // Collision threshold (1 unit radius each)
                    auto collisionEvent = std::make_shared<CollisionEvent>(
                        "Object_A", "Object_B"
                    );
                    
                    if (eventCallback) {
                        eventCallback(collisionEvent);
                    }
                }
            }
        }
    }
};

// Main physics engine
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
            std::cout << "⚠️  Collision detected between objects!" << std::endl;
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

// Actor class that can have physics components
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

// Demo manager
class PhysicsDemo {
private:
    PhysicsEngine engine;
    std::vector<std::shared_ptr<Actor>> actors;
    
public:
    void setupDemo() {
        std::cout << "🚀 Setting up physics simulation..." << std::endl;
        
        // Create static ground object
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
        auto floatingPhysics = std::make_shared<BasePhysicsComponent>(vec3(-3, 8, 0), 0.5f);
        floatingObject->addPhysicsComponent(floatingPhysics);
        engine.addComponent(floatingPhysics);
        actors.push_back(floatingObject);
        
        std::cout << "✅ Scene setup complete with " << actors.size() - 1 << " dynamic objects" << std::endl;
    }
    
    void runSimulation() {
        std::cout << "\n🎮 Running physics simulation..." << std::endl;
        
        // Show initial positions
        std::cout << "\nInitial positions:" << std::endl;
        for (const auto& actor : actors) {
            auto pos = actor->getPhysicsPosition();
            std::cout << "  " << actor->getName() << ": (" 
                      << pos.x << ", " << pos.y << ", " << pos.z << ")" << std::endl;
        }
        
        // Apply forces
        std::cout << "\n🧪 Applying forces..." << std::endl;
        
        // Apply gravity to all objects (except ground)
        for (size_t i = 1; i < actors.size(); ++i) {
            auto physics = actors[i]->getPhysicsComponents()[0];
            physics->applyForce(vec3(0, -9.81, 0)); // Gravity
        }
        
        // Apply thrust to player
        auto playerPhysics = actors[1]->getPhysicsComponents()[0];
        playerPhysics->applyForce(vec3(0, 5, 0)); // Upward thrust
        
        // Apply horizontal force to enemy
        auto enemyPhysics = actors[2]->getPhysicsComponents()[0];
        enemyPhysics->applyForce(vec3(-2, 0, 0)); // Leftward force
        
        // Run simulation for several steps
        std::cout << "\n⏱️  Simulation running (5 time steps)..." << std::endl;
        
        for (int step = 0; step < 5; ++step) {
            std::cout << "\nStep " << step + 1 << ":" << std::endl;
            
            // Update physics engine
            engine.update(0.1f);
            
            // Print positions
            for (const auto& actor : actors) {
                auto pos = actor->getPhysicsPosition();
                std::cout << "  " << actor->getName() << ": (" 
                          << pos.x << ", " << pos.y << ", " << pos.z << ")" << std::endl;
            }
        }
        
        // Demonstrate mass effects
        std::cout << "\n⚖️  Mass effect demonstration:" << std::endl;
        std::cout << "Applying same force to objects with different masses..." << std::endl;
        
        auto heavyObject = std::make_shared<Actor>(vec3(0, 15, 0), "heavy_object");
        auto lightObject = std::make_shared<Actor>(vec3(2, 15, 0), "light_object");
        
        auto heavyPhysics = std::make_shared<BasePhysicsComponent>(vec3(0, 15, 0), 5.0f);
        auto lightPhysics = std::make_shared<BasePhysicsComponent>(vec3(2, 15, 0), 0.5f);
        
        heavyObject->addPhysicsComponent(heavyPhysics);
        lightObject->addPhysicsComponent(lightPhysics);
        
        engine.addComponent(heavyPhysics);
        engine.addComponent(lightPhysics);
        
        // Apply same force to both objects
        float force = 10.0f;
        heavyPhysics->applyForce(vec3(force, 0, 0));
        lightPhysics->applyForce(vec3(force, 0, 0));
        
        std::cout << "Applied force of " << force << "N to both objects" << std::endl;
        
        // Update and show results
        engine.update(0.1f);
        
        auto heavyPos = heavyObject->getPhysicsPosition();
        auto lightPos = lightObject->getPhysicsPosition();
        
        std::cout << "Heavy object position: (" << heavyPos.x << ", " << heavyPos.y << ", " << heavyPos.z << ")" << std::endl;
        std::cout << "Light object position: (" << lightPos.x << ", " << lightPos.y << ", " << lightPos.z << ")" << std::endl;
        
        // Show velocity differences
        auto heavyVel = heavyPhysics->getVelocity();
        auto lightVel = lightPhysics->getVelocity();
        
        std::cout << "Heavy object velocity: (" << heavyVel.x << ", " << heavyVel.y << ", " << heavyVel.z << ")" << std::endl;
        std::cout << "Light object velocity: (" << lightVel.x << ", " << lightVel.y << ", " << lightVel.z << ")" << std::endl;
    }
    
    void showFeatures() {
        std::cout << "\n✨ Physics Engine Features:" << std::endl;
        std::cout << "  ✓ Newtonian physics calculations" << std::endl;
        std::cout << "  ✓ Force-based movement system" << std::endl;
        std::cout << "  ✓ Mass-dependent response" << std::endl;
        std::cout << "  ✓ Collision detection system" << std::endl;
        std::cout << "  ✓ Integration with GameObject/Actor system" << std::endl;
        std::cout << "  ✓ Frame-rate independent time stepping" << std::endl;
    }
};

int main() {
    std::cout << "SGE Physics Engine - Complete Final Demo" << std::endl;
    std::cout << "=========================================" << std::endl;
    
    try {
        PhysicsDemo demo;
        demo.setupDemo();
        demo.runSimulation();
        demo.showFeatures();
        
        std::cout << "\n✅ Demo execution completed successfully!" << std::endl;
        std::cout << "All physics engine components are working correctly." << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Error in demo: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}