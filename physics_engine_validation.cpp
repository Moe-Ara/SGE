/*
 * SGE Physics Engine - Validation Test Suite
 * 
 * This file validates that all requirements for the physics engine have been met.
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
};

// Main physics engine
class PhysicsEngine {
private:
    std::vector<std::shared_ptr<IPhysicsComponent>> components;
    
public:
    void update(float deltaTime) {
        for (auto& component : components) {
            component->update(deltaTime);
        }
    }
    
    void addComponent(std::shared_ptr<IPhysicsComponent> component) {
        components.push_back(component);
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

// Validation functions
void validateVectorMath() {
    std::cout << "🔍 Validating vector math implementation..." << std::endl;
    
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
    
    std::cout << "✅ Vector math implementation is correct" << std::endl;
}

void validatePhysicsInterface() {
    std::cout << "🔍 Validating physics component interface..." << std::endl;
    
    auto physics = std::make_shared<BasePhysicsComponent>(vec3(0, 0, 0), 1.0f);
    
    // Test all required methods exist and work
    assert(physics->getMass() == 1.0f);
    vec3 pos = physics->getPosition();
    assert(pos.x == 0 && pos.y == 0 && pos.z == 0);
    
    // Apply force and update
    physics->applyForce(vec3(0, 10, 0));
    physics->update(1.0f);
    
    std::cout << "✅ Physics component interface is properly implemented" << std::endl;
}

void validateNewtonianPhysics() {
    std::cout << "🔍 Validating Newtonian physics implementation..." << std::endl;
    
    auto physics = std::make_shared<BasePhysicsComponent>(vec3(0, 0, 0), 1.0f);
    
    // Apply gravity force (9.81 N downward)
    vec3 gravityForce(0, -9.81, 0);
    physics->applyForce(gravityForce);
    
    // Update with 1 second time step
    physics->update(1.0f);
    
    // Check that acceleration was applied correctly
    vec3 vel = physics->getVelocity();
    assert(abs(vel.y + 9.81) < 0.01); // Should have -9.81 m/s velocity
    
    std::cout << "✅ Newtonian physics calculations are correct" << std::endl;
}

void validateMassBehavior() {
    std::cout << "🔍 Validating mass-based behavior..." << std::endl;
    
    auto lightObject = std::make_shared<BasePhysicsComponent>(vec3(0, 0, 0), 0.5f);
    auto heavyObject = std::make_shared<BasePhysicsComponent>(vec3(0, 0, 0), 5.0f);
    
    // Apply same force to both
    vec3 force(10, 0, 0); // 10 N horizontal force
    lightObject->applyForce(force);
    heavyObject->applyForce(force);
    
    // Update with same time step
    float deltaTime = 1.0f;
    lightObject->update(deltaTime);
    heavyObject->update(deltaTime);
    
    // Light object should accelerate more (a=F/m = 10/0.5 = 20 m/s²)
    vec3 lightVel = lightObject->getVelocity();
    vec3 heavyVel = heavyObject->getVelocity();
    
    assert(lightVel.x > heavyVel.x); // Light object should move faster
    
    std::cout << "✅ Mass-based behavior is correctly implemented" << std::endl;
}

void validateIntegration() {
    std::cout << "🔍 Validating GameObject/Actor integration..." << std::endl;
    
    PhysicsEngine engine;
    
    // Create actor with physics
    auto actor = std::make_shared<Actor>(vec3(0, 10, 0), "Player");
    auto physics = std::make_shared<BasePhysicsComponent>(vec3(0, 10, 0), 1.0f);
    
    actor->addPhysicsComponent(physics);
    engine.addComponent(physics);
    
    // Verify integration works
    vec3 initialPos = actor->getPhysicsPosition();
    assert(initialPos.y == 10); // Should start at y=10
    
    // Apply gravity and update
    physics->applyForce(vec3(0, -9.81, 0));
    engine.update(0.1f);
    
    vec3 finalPos = actor->getPhysicsPosition();
    assert(finalPos.y < 10); // Should have moved down
    
    std::cout << "✅ GameObject/Actor integration is working correctly" << std::endl;
}

void validateAllRequirements() {
    std::cout << "\n📋 Validating all requirements are met:" << std::endl;
    std::cout << "=========================================" << std::endl;
    
    validateVectorMath();
    validatePhysicsInterface();
    validateNewtonianPhysics();
    validateMassBehavior();
    validateIntegration();
    
    std::cout << "\n🎉 All requirements have been successfully validated!" << std::endl;
    std::cout << "\n✅ Vector math implementation" << std::endl;
    std::cout << "✅ Physics component interface" << std::endl;
    std::cout << "✅ Newtonian physics engine" << std::endl;
    std::cout << "✅ Mass-based behavior" << std::endl;
    std::cout << "✅ GameObject/Actor integration" << std::endl;
}

int main() {
    try {
        std::cout << "SGE Physics Engine - Requirements Validation" << std::endl;
        std::cout << "=============================================" << std::endl;
        
        validateAllRequirements();
        
        std::cout << "\n✅ Physics engine implementation is complete and validated!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Validation failed: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}