#ifndef PHYSICS_SYSTEM_H
#define PHYSICS_SYSTEM_H

#include "position.h"
#include <vector>
#include <memory>
#include <functional>

// Forward declarations
class Character;
class Mob;
class Collider;

// Physics body types
enum class BodyType {
    STATIC,     // Immovable objects (walls, terrain)
    DYNAMIC,    // Movable objects (characters, projectiles)
    KINEMATIC   // Script-controlled objects (doors, platforms)
};

// Physics material properties
struct PhysicsMaterial {
    float friction;
    float restitution;  // Bounciness
    float density;
    
    PhysicsMaterial(float f = 0.5f, float r = 0.0f, float d = 1.0f)
        : friction(f), restitution(r), density(d) {}
};

// Physics body for collision detection
struct PhysicsBody {
    Position position;
    Position velocity;
    Position acceleration;
    Position force;
    
    float mass;
    float invMass;  // Inverse mass (1/mass) for performance
    float linearDamping;
    
    BodyType bodyType;
    PhysicsMaterial material;
    
    // Collision detection
    std::shared_ptr<Collider> collider;
    bool isTrigger;
    bool isActive;
    
    // Callbacks
    std::function<void(PhysicsBody*)> onCollisionEnter;
    std::function<void(PhysicsBody*)> onCollisionStay;
    std::function<void(PhysicsBody*)> onCollisionExit;
    
    PhysicsBody() : position(0, 0, 0), velocity(0, 0, 0), acceleration(0, 0, 0),
                    force(0, 0, 0), mass(1.0f), invMass(1.0f), linearDamping(0.01f),
                    bodyType(BodyType::DYNAMIC), material(), isTrigger(false), isActive(true) {}
};

// Collision detection interface
class Collider {
public:
    virtual ~Collider() = default;
    virtual bool checkCollision(const Collider* other) const = 0;
    virtual Position getCenter() const = 0;
    virtual float getRadius() const = 0;
    virtual void updateTransform(const Position& position) = 0;
};

// Sphere collider
class SphereCollider : public Collider {
private:
    Position center;
    float radius;
    
public:
    SphereCollider(const Position& pos = Position(0, 0, 0), float r = 1.0f)
        : center(pos), radius(r) {}
    
    bool checkCollision(const Collider* other) const override;
    Position getCenter() const override { return center; }
    float getRadius() const override { return radius; }
    void updateTransform(const Position& position) override { center = position; }
    
    void setRadius(float r) { radius = r; }
};

// AABB collider (Axis-Aligned Bounding Box)
class AABBCollider : public Collider {
private:
    Position min;
    Position max;
    
public:
    AABBCollider(const Position& minPos = Position(-1, -1, -1), 
                  const Position& maxPos = Position(1, 1, 1))
        : min(minPos), max(maxPos) {}
    
    bool checkCollision(const Collider* other) const override;
    Position getCenter() const override;
    float getRadius() const override;
    void updateTransform(const Position& position) override;
    
    void setBounds(const Position& minPos, const Position& maxPos) { min = minPos; max = maxPos; }
    Position getMin() const { return min; }
    Position getMax() const { return max; }
};

// Physics system main class
class PhysicsSystem {
private:
    std::vector<std::shared_ptr<PhysicsBody>> bodies;
    
    // Physics constants
    const float GRAVITY = -9.81f;
    const float MAX_VELOCITY = 50.0f;
    const float VELOCITY_THRESHOLD = 0.01f;
    
    // Spatial partitioning for collision detection
    struct SpatialCell {
        std::vector<std::shared_ptr<PhysicsBody>> bodies;
    };
    
    std::vector<std::vector<std::vector<SpatialCell>>> spatialGrid;
    float cellSize;
    Position gridBounds;
    
public:
    PhysicsSystem();
    ~PhysicsSystem();
    
    // Core physics loop
    void update(float deltaTime);
    void simulatePhysics(float deltaTime);
    void detectCollisions();
    void resolveCollisions();
    
    // Body management
    std::shared_ptr<PhysicsBody> createBody(const Position& position, float mass = 1.0f);
    void addBody(std::shared_ptr<PhysicsBody> body);
    void removeBody(std::shared_ptr<PhysicsBody> body);
    void clearAllBodies();
    
    // Collision detection
    bool checkCollision(const PhysicsBody* body1, const PhysicsBody* body2) const;
    std::vector<std::shared_ptr<PhysicsBody>> getBodiesInRadius(const Position& center, float radius);
    
    // Physics queries
    bool raycast(const Position& start, const Position& direction, float maxDistance, 
                 std::shared_ptr<PhysicsBody>& hitBody, Position& hitPoint);
    bool lineOfSight(const Position& start, const Position& end);
    
    // Spatial partitioning
    void updateSpatialGrid();
    void setGridParameters(float cellSize, const Position& bounds);
    
    // Utility methods
    void setGravity(float gravity) { /* TODO: Implement */ }
    void setMaxVelocity(float maxVel) { /* TODO: Implement */ }
    size_t getBodyCount() const { return bodies.size(); }
    
private:
    // Helper methods
    void integrateVelocity(std::shared_ptr<PhysicsBody> body, float deltaTime);
    void applyForces(std::shared_ptr<PhysicsBody> body, float deltaTime);
    void clampVelocity(std::shared_ptr<PhysicsBody> body);
    void updateBodyTransform(std::shared_ptr<PhysicsBody> body, float deltaTime);
    
    // Spatial partitioning helpers
    void addBodyToGrid(std::shared_ptr<PhysicsBody> body);
    void removeBodyFromGrid(std::shared_ptr<PhysicsBody> body);
    std::vector<std::tuple<int, int, int>> getGridIndices(const Position& position, float radius);
};

#endif // PHYSICS_SYSTEM_H
