/*
 * SGE Physics Engine - Test Suite
 * 
 * Comprehensive test suite for validating the physics engine implementation
 */

#include <iostream>
#include <vector>
#include <memory>
#include <cassert>
#include <chrono>

// Mock vector math library (simplified version)
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
    
    bool operator==(const vec3& other) const {
        return (x == other.x) && (y == other.y) && (z == other.z);
    }
};

// Forward declarations
class GameObject;
class IPhysicsComponent;
class PhysicsEngine;

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
    
public:
    BasePhysicsComponent(float m = 1.0f, const vec3& pos = vec3(0, 0, 0))
        : position(pos), velocity(vec3(0, 0, 0)), 
          acceleration(vec3(0, 0, 0)), mass(m) {}
    
    void update(float deltaTime) override {
        // Integrate velocity from acceleration
        velocity = velocity + acceleration * deltaTime;
        
        // Integrate position from velocity  
        position = position + velocity * deltaTime;
        
        // Reset acceleration for next frame
        acceleration = vec3(0, 0, 0);
    }
    
    void applyForce(const vec3& force) {
        acceleration = acceleration + force / mass;
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
};

// Physics engine coordinator
class PhysicsEngine {
private:
    std::vector<std::shared_ptr<IPhysicsComponent>> physicsComponents;
    std::unique_ptr<SimpleCollisionSystem> collisionSystem;
    
public:
    PhysicsEngine() {
        collisionSystem = std::make_unique<SimpleCollisionSystem>();
    }
    
    void update(float deltaTime) {
        for (auto& component : physicsComponents) {
            component->update(deltaTime);
        }
    }
    
    void addPhysicsComponent(std::shared_ptr<IPhysicsComponent> component) {
        physicsComponents.push_back(component);
    }
    
    void removePhysicsComponent(std::shared_ptr<IPhysicsComponent> component) {
        auto it = std::find(physicsComponents.begin(), physicsComponents.end(), component);
        if (it != physicsComponents.end()) {
            physicsComponents.erase(it);
        }
    }
    
    bool checkCollision(const IPhysicsComponent& a, const IPhysicsComponent& b) {
        return collisionSystem->checkCollision(a, b);
    }
};

// Test functions
void testPhysicsComponentInitialization() {
    std::cout << "Testing Physics Component Initialization..." << std::endl;
    
    auto component = std::make_shared<BasePhysicsComponent>(5.0f, vec3(1, 2, 3));
    
    assert(component->getMass() == 5.0f);
    assert(component->getPosition() == vec3(1, 2, 3));
    assert(component->getVelocity() == vec3(0, 0, 0));
    
    std::cout << "✓ Physics component initialized correctly" << std::endl;
}

void testPhysicsUpdate() {
    std::cout << "Testing Physics Update..." << std::endl;
    
    auto component = std::make_shared<BasePhysicsComponent>(1.0f, vec3(0, 0, 0));
    
    // Set initial velocity
    component->setVelocity(vec3(1, 2, 3));
    
    // Update with delta time
    component->update(1.0f);
    
    // Verify position updated based on velocity
    vec3 expectedPos = vec3(1, 2, 3);
    assert(component->getPosition() == expectedPos);
    
    std::cout << "✓ Physics update works correctly" << std::endl;
}

void testForceApplication() {
    std::cout << "Testing Force Application..." << std::endl;
    
    auto component = std::make_shared<BasePhysicsComponent>(2.0f, vec3(0, 0, 0));
    
    // Apply force
    vec3 force(0, 10, 0);
    component->applyForce(force);
    
    // Update to see effect
    component->update(1.0f);
    
    // Verify velocity changed based on force and mass
    // Force = mass * acceleration => acceleration = force / mass = 10/2 = 5
    // Velocity = initial_velocity + acceleration * deltaTime = 0 + 5*1 = 5
    vec3 expectedVel(0, 5, 0);
    assert(component->getVelocity() == expectedVel);
    
    std::cout << "✓ Force application works correctly" << std::endl;
}

void testPhysicsEngineIntegration() {
    std::cout << "Testing Physics Engine Integration..." << std::endl;
    
    PhysicsEngine engine;
    
    auto component1 = std::make_shared<BasePhysicsComponent>(1.0f, vec3(0, 0, 0));
    auto component2 = std::make_shared<BasePhysicsComponent>(1.0f, vec3(0.5f, 0, 0));
    
    engine.addPhysicsComponent(component1);
    engine.addPhysicsComponent(component2);
    
    // Update engine
    engine.update(1.0f);
    
    assert(component1->getPosition() == vec3(0, 0, 0)); // No force applied
    assert(component2->getPosition() == vec3(0.5f, 0, 0)); // No force applied
    
    std::cout << "✓ Physics engine integration works correctly" << std::endl;
}

void testCollisionDetection() {
    std::cout << "Testing Collision Detection..." << std::endl;
    
    PhysicsEngine engine;
    
    auto component1 = std::make_shared<BasePhysicsComponent>(1.0f, vec3(0, 0, 0));
    auto component2 = std::make_shared<BasePhysicsComponent>(1.0f, vec3(0.5f, 0, 0));
    
    // Test collision - should detect as colliding
    bool collision = engine.checkCollision(*component1, *component2);
    assert(collision == true);
    
    auto component3 = std::make_shared<BasePhysicsComponent>(1.0f, vec3(2, 0, 0));
    
    // Test no collision - should not detect as colliding
    bool noCollision = engine.checkCollision(*component1, *component3);
    assert(noCollision == false);
    
    std::cout << "✓ Collision detection works correctly" << std::endl;
}

void testMassEffect() {
    std::cout << "Testing Mass Effect on Physics..." << std::endl;
    
    auto lightComponent = std::make_shared<BasePhysicsComponent>(1.0f, vec3(0, 0, 0));
    auto heavyComponent = std::make_shared<BasePhysicsComponent>(10.0f, vec3(0, 0, 0));
    
    // Apply same force to both
    vec3 force(0, 10, 0);
    lightComponent->applyForce(force);
    heavyComponent->applyForce(force);
    
    // Update
    lightComponent->update(1.0f);
    heavyComponent->update(1.0f);
    
    // Light component should accelerate more (a = F/m)
    vec3 lightVel = lightComponent->getVelocity();
    vec3 heavyVel = heavyComponent->getVelocity();
    
    assert(lightVel.y > heavyVel.y); // Light should have higher velocity
    
    std::cout << "✓ Mass effect works correctly" << std::endl;
}

void testPerformance() {
    std::cout << "Testing Performance..." << std::endl;
    
    PhysicsEngine engine;
    
    // Create many components
    const int numComponents = 1000;
    std::vector<std::shared_ptr<BasePhysicsComponent>> components;
    
    for (int i = 0; i < numComponents; ++i) {
        auto component = std::make_shared<BasePhysicsComponent>(1.0f, vec3(i, 0, 0));
        engine.addPhysicsComponent(component);
        components.push_back(component);
    }
    
    // Time the update
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 10; ++i) {
        engine.update(0.1f);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    std::cout << "✓ Performance test completed in " << duration.count() << " microseconds" << std::endl;
}

void runAllTests() {
    std::cout << "Running SGE Physics Engine Test Suite..." << std::endl << std::endl;
    
    try {
        testPhysicsComponentInitialization();
        testPhysicsUpdate();
        testForceApplication();
        testPhysicsEngineIntegration();
        testCollisionDetection();
        testMassEffect();
        testPerformance();
        
        std::cout << std::endl << "All tests passed! ✓" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Test failed: " << e.what() << std::endl;
        throw;
    }
}

int main() {
    try {
        runAllTests();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}