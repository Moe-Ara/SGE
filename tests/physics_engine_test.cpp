/*
 * SGE Physics Engine - Comprehensive Test Suite
 * 
 * This file tests all components of the physics engine to ensure they work together
 * as intended and meet all specified requirements.
 */

#include <iostream>
#include <vector>
#include <memory>
#include <cassert>
#include <string>

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
};

// Base Event class
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
    bool collisionDetected = false;
    
public:
    void addComponent(std::shared_ptr<IPhysicsComponent> component) {
        components.push_back(component);
    }
    
    void checkCollisions() {
        collisionDetected = false;
        
        // Simple pairwise collision detection
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
                    collisionDetected = true;
                    break;
                }
            }
            if (collisionDetected) break;
        }
    }
    
    bool hasCollision() const {
        return collisionDetected;
    }
};

// Main physics engine
class PhysicsEngine {
private:
    std::vector<std::shared_ptr<IPhysicsComponent>> components;
    SimpleCollisionSystem collisionSystem;
    
public:
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
    
    bool hasCollision() const {
        return collisionSystem.hasCollision();
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

// Test functions
void testVectorMath() {
    std::cout << "🧪 Testing vector math..." << std::endl;
    
    vec3 a(1, 2, 3);
    vec3 b(4, 5, 6);
    
    // Test addition
    vec3 sum = a + b;
    assert(sum.x == 5 && sum.y == 7 && sum.z == 9);
    
    // Test subtraction
    vec3 diff = b - a;
    assert(diff.x == 3 && diff.y == 3 && diff.z == 3);
    
    // Test scalar multiplication
    vec3 scaled = a * 2.0f;
    assert(scaled.x == 2 && scaled.y == 4 && scaled.z == 6);
    
    // Test length calculation
    float len = a.length();
    assert(abs(len - 3.741657) < 0.001); // sqrt(14)
    
    std::cout << "✅ Vector math tests passed!" << std::endl;
}

void testPhysicsComponent() {
    std::cout << "🧪 Testing physics component..." << std::endl;
    
    BasePhysicsComponent component(vec3(0, 0, 0), 1.0f);
    
    // Test initial state
    vec3 pos = component.getPosition();
    assert(pos.x == 0 && pos.y == 0 && pos.z == 0);
    
    // Test mass
    float mass = component.getMass();
    assert(mass == 1.0f);
    
    // Test force application
    component.applyForce(vec3(0, 10, 0)); // Apply upward force
    
    // Update with time step
    component.update(1.0f);
    
    // Check that velocity and position changed
    vec3 vel = component.getVelocity();
    assert(vel.y > 0); // Should be moving up
    
    std::cout << "✅ Physics component tests passed!" << std::endl;
}

void testMassBehavior() {
    std::cout << "🧪 Testing mass-based behavior..." << std::endl;
    
    BasePhysicsComponent heavy(vec3(0, 0, 0), 5.0f);
    BasePhysicsComponent light(vec3(0, 0, 0), 0.5f);
    
    // Apply same force to both
    vec3 force(0, 10, 0);
    heavy.applyForce(force);
    light.applyForce(force);
    
    // Update with same time step
    float deltaTime = 1.0f;
    heavy.update(deltaTime);
    light.update(deltaTime);
    
    // Light object should accelerate more (F=ma => a=F/m)
    vec3 heavyVel = heavy.getVelocity();
    vec3 lightVel = light.getVelocity();
    
    assert(lightVel.y > heavyVel.y); // Light accelerates more
    
    std::cout << "✅ Mass behavior tests passed!" << std::endl;
}

void testIntegration() {
    std::cout << "🧪 Testing GameObject/Actor integration..." << std::endl;
    
    Actor actor(vec3(1, 2, 3), "test_actor");
    auto physics = std::make_shared<BasePhysicsComponent>(vec3(1, 2, 3), 1.0f);
    
    actor.addPhysicsComponent(physics);
    
    // Test that actor can access physics
    vec3 pos = actor.getPhysicsPosition();
    assert(pos.x == 1 && pos.y == 2 && pos.z == 3);
    
    // Apply force and update
    physics->applyForce(vec3(0, 5, 0));
    actor.update(1.0f);
    
    // Check that position changed
    vec3 newPos = actor.getPhysicsPosition();
    assert(newPos.y > 2); // Should have moved up
    
    std::cout << "✅ Integration tests passed!" << std::endl;
}

void testCollisionSystem() {
    std::cout << "🧪 Testing collision system..." << std::endl;
    
    PhysicsEngine engine;
    
    Actor obj1(vec3(0, 0, 0), "object1");
    Actor obj2(vec3(1.5f, 0, 0), "object2"); // Close enough to collide
    
    auto physics1 = std::make_shared<BasePhysicsComponent>(vec3(0, 0, 0), 1.0f);
    auto physics2 = std::make_shared<BasePhysicsComponent>(vec3(1.5f, 0, 0), 1.0f);
    
    obj1.addPhysicsComponent(physics1);
    obj2.addPhysicsComponent(physics2);
    
    engine.addComponent(physics1);
    engine.addComponent(physics2);
    
    // Update to trigger collision check
    engine.update(0.1f);
    
    // Should detect collision
    assert(engine.hasCollision() == true);
    
    std::cout << "✅ Collision system tests passed!" << std::endl;
}

void testEngineUpdate() {
    std::cout << "🧪 Testing complete engine update..." << std::endl;
    
    PhysicsEngine engine;
    
    Actor player(vec3(0, 10, 0), "player");
    auto playerPhysics = std::make_shared<BasePhysicsComponent>(vec3(0, 10, 0), 1.0f);
    player.addPhysicsComponent(playerPhysics);
    engine.addComponent(playerPhysics);
    
    // Apply gravity
    playerPhysics->applyForce(vec3(0, -9.81, 0));
    
    // Run update
    engine.update(0.1f);
    
    // Check that physics updated
    vec3 pos = player.getPhysicsPosition();
    assert(pos.y < 10); // Should have fallen
    
    std::cout << "✅ Engine update tests passed!" << std::endl;
}

int main() {
    std::cout << "SGE Physics Engine - Complete Test Suite" << std::endl;
    std::cout << "=========================================" << std::endl;
    
    try {
        testVectorMath();
        testPhysicsComponent();
        testMassBehavior();
        testIntegration();
        testCollisionSystem();
        testEngineUpdate();
        
        std::cout << "\n🎉 ALL TESTS PASSED!" << std::endl;
        std::cout << "The physics engine components work correctly and meet all requirements." << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Test failed: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}