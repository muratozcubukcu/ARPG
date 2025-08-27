#include "physics_system.h"
#include <iostream>
#include <algorithm>

PhysicsSystem::PhysicsSystem() : cellSize(10.0f), gridBounds(100.0f, 100.0f, 20.0f) {
    std::cout << "PhysicsSystem initialized" << std::endl;
    
    // Initialize spatial grid
    int gridX = static_cast<int>(gridBounds.getX() / cellSize) + 1;
    int gridY = static_cast<int>(gridBounds.getY() / cellSize) + 1;
    int gridZ = static_cast<int>(gridBounds.getZ() / cellSize) + 1;
    
    spatialGrid.resize(gridX);
    for (auto& xLayer : spatialGrid) {
        xLayer.resize(gridY);
        for (auto& yLayer : xLayer) {
            yLayer.resize(gridZ);
        }
    }
}

PhysicsSystem::~PhysicsSystem() {
    std::cout << "PhysicsSystem destroyed" << std::endl;
}

void PhysicsSystem::update(float deltaTime) {
    simulatePhysics(deltaTime);
    detectCollisions();
    resolveCollisions();
    updateSpatialGrid();
}

void PhysicsSystem::simulatePhysics(float deltaTime) {
    for (auto& body : bodies) {
        if (!body->isActive || body->bodyType == BodyType::STATIC) continue;
        
        applyForces(body, deltaTime);
        integrateVelocity(body, deltaTime);
        clampVelocity(body);
        updateBodyTransform(body, deltaTime);
    }
}

void PhysicsSystem::detectCollisions() {
    // Simple broad phase collision detection using spatial grid
    for (size_t i = 0; i < bodies.size(); ++i) {
        if (!bodies[i]->isActive) continue;
        
        for (size_t j = i + 1; j < bodies.size(); ++j) {
            if (!bodies[j]->isActive) continue;
            
            if (checkCollision(bodies[i].get(), bodies[j].get())) {
                // Collision detected - trigger callbacks
                if (bodies[i]->onCollisionEnter) {
                    bodies[i]->onCollisionEnter(bodies[j].get());
                }
                if (bodies[j]->onCollisionEnter) {
                    bodies[j]->onCollisionEnter(bodies[i].get());
                }
            }
        }
    }
}

void PhysicsSystem::resolveCollisions() {
    // Simple collision resolution - push bodies apart
    for (size_t i = 0; i < bodies.size(); ++i) {
        if (!bodies[i]->isActive || bodies[i]->bodyType == BodyType::STATIC) continue;
        
        for (size_t j = i + 1; j < bodies.size(); ++j) {
            if (!bodies[j]->isActive || bodies[j]->bodyType == BodyType::STATIC) continue;
            
            if (checkCollision(bodies[i].get(), bodies[j].get())) {
                // Calculate separation vector
                Position separation = bodies[i]->position - bodies[j]->position;
                double distance = separation.length();
                
                if (distance > 0) {
                    double minDistance = 1.0; // Minimum separation distance
                    double overlap = minDistance - distance;
                    
                    if (overlap > 0) {
                        Position separationDir = separation.normalize();
                        Position correction = separationDir * (overlap * 0.5);
                        
                        // Move bodies apart
                        if (bodies[i]->bodyType == BodyType::DYNAMIC) {
                            bodies[i]->position = bodies[i]->position + correction;
                        }
                        if (bodies[j]->bodyType == BodyType::DYNAMIC) {
                            bodies[j]->position = bodies[j]->position - correction;
                        }
                    }
                }
            }
        }
    }
}

std::shared_ptr<PhysicsBody> PhysicsSystem::createBody(const Position& position, float mass) {
    auto body = std::make_shared<PhysicsBody>();
    body->position = position;
    body->mass = mass;
    body->invMass = (mass > 0) ? 1.0f / mass : 0.0f;
    
    // Create default sphere collider
    body->collider = std::make_shared<SphereCollider>(position, 1.0f);
    
    addBody(body);
    return body;
}

void PhysicsSystem::addBody(std::shared_ptr<PhysicsBody> body) {
    bodies.push_back(body);
    addBodyToGrid(body);
}

void PhysicsSystem::removeBody(std::shared_ptr<PhysicsBody> body) {
    removeBodyFromGrid(body);
    bodies.erase(std::remove(bodies.begin(), bodies.end(), body), bodies.end());
}

void PhysicsSystem::clearAllBodies() {
    bodies.clear();
    // Clear spatial grid
    for (auto& xLayer : spatialGrid) {
        for (auto& yLayer : xLayer) {
            for (auto& zLayer : yLayer) {
                zLayer.bodies.clear();
            }
        }
    }
}

bool PhysicsSystem::checkCollision(const PhysicsBody* body1, const PhysicsBody* body2) const {
    if (!body1->collider || !body2->collider) return false;
    return body1->collider->checkCollision(body2->collider.get());
}

std::vector<std::shared_ptr<PhysicsBody>> PhysicsSystem::getBodiesInRadius(const Position& center, float radius) {
    std::vector<std::shared_ptr<PhysicsBody>> nearbyBodies;
    
    for (const auto& body : bodies) {
        if (!body->isActive) continue;
        
        double distance = center.distanceTo(body->position);
        if (distance <= radius) {
            nearbyBodies.push_back(body);
        }
    }
    
    return nearbyBodies;
}

bool PhysicsSystem::raycast(const Position& start, const Position& direction, float maxDistance, 
                           std::shared_ptr<PhysicsBody>& hitBody, Position& hitPoint) {
    // Simple raycast implementation
    Position end = start + direction.normalize() * maxDistance;
    
    for (const auto& body : bodies) {
        if (!body->isActive) continue;
        
        // Check if ray intersects with body's collider
        if (body->collider) {
            Position center = body->collider->getCenter();
            float radius = body->collider->getRadius();
            
            // Simple sphere-ray intersection
            Position toCenter = center - start;
            double projection = toCenter.dot(direction);
            
            if (projection < 0) continue; // Ray points away from sphere
            
            double closestApproach = toCenter.dot(toCenter) - projection * projection;
            double radiusSquared = radius * radius;
            
            if (closestApproach > radiusSquared) continue; // Ray misses sphere
            
            double halfChord = sqrt(radiusSquared - closestApproach);
            double distance = projection - halfChord;
            
            if (distance > 0 && distance <= maxDistance) {
                hitBody = body;
                hitPoint = start + direction.normalize() * distance;
                return true;
            }
        }
    }
    
    return false;
}

bool PhysicsSystem::lineOfSight(const Position& start, const Position& end) {
    std::shared_ptr<PhysicsBody> hitBody;
    Position hitPoint;
    
    Position direction = end - start;
    float distance = direction.length();
    
    return !raycast(start, direction, distance, hitBody, hitPoint);
}

void PhysicsSystem::updateSpatialGrid() {
    // Clear grid
    for (auto& xLayer : spatialGrid) {
        for (auto& yLayer : xLayer) {
            for (auto& zLayer : yLayer) {
                zLayer.bodies.clear();
            }
        }
    }
    
    // Re-add all bodies to grid
    for (const auto& body : bodies) {
        if (body->isActive) {
            addBodyToGrid(body);
        }
    }
}

void PhysicsSystem::setGridParameters(float cellSize, const Position& bounds) {
    this->cellSize = cellSize;
    this->gridBounds = bounds;
    
    // Reinitialize grid with new parameters
    int gridX = static_cast<int>(gridBounds.getX() / cellSize) + 1;
    int gridY = static_cast<int>(gridBounds.getY() / cellSize) + 1;
    int gridZ = static_cast<int>(gridBounds.getZ() / cellSize) + 1;
    
    spatialGrid.clear();
    spatialGrid.resize(gridX);
    for (auto& xLayer : spatialGrid) {
        xLayer.resize(gridY);
        for (auto& yLayer : xLayer) {
            yLayer.resize(gridZ);
        }
    }
}

void PhysicsSystem::integrateVelocity(std::shared_ptr<PhysicsBody> body, float deltaTime) {
    // Apply velocity to position
    body->position = body->position + body->velocity * deltaTime;
    
    // Apply damping
    body->velocity = body->velocity * (1.0f - body->linearDamping * deltaTime);
}

void PhysicsSystem::applyForces(std::shared_ptr<PhysicsBody> body, float deltaTime) {
    // Apply gravity
    body->force.setZ(body->force.getZ() + GRAVITY * body->mass);
    
    // Calculate acceleration
    body->acceleration = body->force * body->invMass;
    
    // Apply acceleration to velocity
    body->velocity = body->velocity + body->acceleration * deltaTime;
    
    // Reset forces
    body->force = Position(0, 0, 0);
}

void PhysicsSystem::clampVelocity(std::shared_ptr<PhysicsBody> body) {
    float velocityMagnitude = body->velocity.length();
    if (velocityMagnitude > MAX_VELOCITY) {
        body->velocity = body->velocity.normalize() * MAX_VELOCITY;
    }
    
    // Apply velocity threshold
    if (velocityMagnitude < VELOCITY_THRESHOLD) {
        body->velocity = Position(0, 0, 0);
    }
}

void PhysicsSystem::updateBodyTransform(std::shared_ptr<PhysicsBody> body, float deltaTime) {
    // Update collider position
    if (body->collider) {
        body->collider->updateTransform(body->position);
    }
}

void PhysicsSystem::addBodyToGrid(std::shared_ptr<PhysicsBody> body) {
    auto indices = getGridIndices(body->position, 1.0f);
    
    for (const auto& index : indices) {
        int x = std::get<0>(index);
        int y = std::get<1>(index);
        int z = std::get<2>(index);
        if (x >= 0 && x < static_cast<int>(spatialGrid.size()) &&
            y >= 0 && y < static_cast<int>(spatialGrid[0].size()) &&
            z >= 0 && z < static_cast<int>(spatialGrid[0][0].size())) {
            spatialGrid[x][y][z].bodies.push_back(body);
        }
    }
}

void PhysicsSystem::removeBodyFromGrid(std::shared_ptr<PhysicsBody> body) {
    auto indices = getGridIndices(body->position, 1.0f);
    
    for (const auto& index : indices) {
        int x = std::get<0>(index);
        int y = std::get<1>(index);
        int z = std::get<2>(index);
        if (x >= 0 && x < static_cast<int>(spatialGrid.size()) &&
            y >= 0 && y < static_cast<int>(spatialGrid[0].size()) &&
            z >= 0 && z < static_cast<int>(spatialGrid[0][0].size())) {
            auto& cell = spatialGrid[x][y][z];
            cell.bodies.erase(std::remove(cell.bodies.begin(), cell.bodies.end(), body), cell.bodies.end());
        }
    }
}

std::vector<std::tuple<int, int, int>> PhysicsSystem::getGridIndices(const Position& position, float radius) {
    std::vector<std::tuple<int, int, int>> indices;
    
    int minX = static_cast<int>((position.getX() - radius) / cellSize);
    int maxX = static_cast<int>((position.getX() + radius) / cellSize);
    int minY = static_cast<int>((position.getY() - radius) / cellSize);
    int maxY = static_cast<int>((position.getY() + radius) / cellSize);
    int minZ = static_cast<int>((position.getZ() - radius) / cellSize);
    int maxZ = static_cast<int>((position.getZ() + radius) / cellSize);
    
    for (int x = minX; x <= maxX; ++x) {
        for (int y = minY; y <= maxY; ++y) {
            for (int z = minZ; z <= maxZ; ++z) {
                indices.emplace_back(x, y, z);
            }
        }
    }
    
    return indices;
}

// Additional utility methods implementation
std::vector<std::shared_ptr<PhysicsBody>> PhysicsSystem::getBodiesInAABB(const Position& min, const Position& max) {
    std::vector<std::shared_ptr<PhysicsBody>> bodiesInAABB;
    
    for (const auto& body : bodies) {
        if (!body->isActive) continue;
        
        Position bodyPos = body->position;
        if (bodyPos.getX() >= min.getX() && bodyPos.getX() <= max.getX() &&
            bodyPos.getY() >= min.getY() && bodyPos.getY() <= max.getY() &&
            bodyPos.getZ() >= min.getZ() && bodyPos.getZ() <= max.getZ()) {
            bodiesInAABB.push_back(body);
        }
    }
    
    return bodiesInAABB;
}

std::vector<std::shared_ptr<PhysicsBody>> PhysicsSystem::getBodiesAtPosition(const Position& position, float tolerance) {
    std::vector<std::shared_ptr<PhysicsBody>> bodiesAtPosition;
    
    for (const auto& body : bodies) {
        if (!body->isActive) continue;
        
        double distance = position.distanceTo(body->position);
        if (distance <= tolerance) {
            bodiesAtPosition.push_back(body);
        }
    }
    
    return bodiesAtPosition;
}

bool PhysicsSystem::isPositionOccupied(const Position& position, float radius) {
    for (const auto& body : bodies) {
        if (!body->isActive) continue;
        
        double distance = position.distanceTo(body->position);
        if (distance < radius) {
            return true;
        }
    }
    return false;
}

void PhysicsSystem::applyImpulse(std::shared_ptr<PhysicsBody> body, const Position& impulse) {
    if (!body || body->bodyType == BodyType::STATIC) return;
    
    // Apply impulse to velocity: v = v + impulse / mass
    body->velocity = body->velocity + impulse * body->invMass;
}

void PhysicsSystem::setBodyCollider(std::shared_ptr<PhysicsBody> body, std::shared_ptr<Collider> collider) {
    if (body && collider) {
        body->collider = collider;
        collider->updateTransform(body->position);
    }
}

void PhysicsSystem::setBodyType(std::shared_ptr<PhysicsBody> body, BodyType type) {
    if (body) {
        body->bodyType = type;
        if (type == BodyType::STATIC) {
            body->velocity = Position(0, 0, 0);
            body->acceleration = Position(0, 0, 0);
            body->force = Position(0, 0, 0);
        }
    }
}

void PhysicsSystem::setBodyMass(std::shared_ptr<PhysicsBody> body, float mass) {
    if (body && mass > 0) {
        body->mass = mass;
        body->invMass = 1.0f / mass;
    } else if (body && mass <= 0) {
        body->mass = 0.0f;
        body->invMass = 0.0f; // Infinite mass (static)
    }
}

// Collider implementations
bool SphereCollider::checkCollision(const Collider* other) const {
    if (const SphereCollider* sphere = dynamic_cast<const SphereCollider*>(other)) {
        double distance = center.distanceTo(sphere->center);
        return distance < (radius + sphere->radius);
    }
    else if (const AABBCollider* aabb = dynamic_cast<const AABBCollider*>(other)) {
        // Sphere-AABB collision detection
        Position aabbCenter = aabb->getCenter();
        Position aabbHalfSize = (aabb->getMax() - aabb->getMin()) * 0.5;
        
        // Find closest point on AABB to sphere center
        Position closestPoint;
        closestPoint.setX(std::max(aabbCenter.getX() - aabbHalfSize.getX(), 
                                  std::min(center.getX(), aabbCenter.getX() + aabbHalfSize.getX())));
        closestPoint.setY(std::max(aabbCenter.getY() - aabbHalfSize.getY(), 
                                  std::min(center.getY(), aabbCenter.getY() + aabbHalfSize.getY())));
        closestPoint.setZ(std::max(aabbCenter.getZ() - aabbHalfSize.getZ(), 
                                  std::min(center.getZ(), aabbCenter.getZ() + aabbHalfSize.getZ())));
        
        // Check if closest point is inside sphere
        double distance = center.distanceTo(closestPoint);
        return distance < radius;
    }
    return false;
}

Position AABBCollider::getCenter() const {
    return Position((min.getX() + max.getX()) * 0.5,
                   (min.getY() + max.getY()) * 0.5,
                   (min.getZ() + max.getZ()) * 0.5);
}

float AABBCollider::getRadius() const {
    Position size = max - min;
    return size.length() * 0.5f;
}

void AABBCollider::updateTransform(const Position& position) {
    Position center = getCenter();
    Position offset = position - center;
    min = min + offset;
    max = max + offset;
}

bool AABBCollider::checkCollision(const Collider* other) const {
    if (const AABBCollider* aabb = dynamic_cast<const AABBCollider*>(other)) {
        return !(max.getX() < aabb->min.getX() || min.getX() > aabb->max.getX() ||
                 max.getY() < aabb->min.getY() || min.getY() > aabb->max.getY() ||
                 max.getZ() < aabb->min.getZ() || min.getZ() > aabb->max.getZ());
    }
    else if (const SphereCollider* sphere = dynamic_cast<const SphereCollider*>(other)) {
        // AABB-Sphere collision detection (same as Sphere-AABB)
        Position aabbCenter = getCenter();
        Position aabbHalfSize = (max - min) * 0.5;
        
        // Find closest point on AABB to sphere center
        Position closestPoint;
        closestPoint.setX(std::max(aabbCenter.getX() - aabbHalfSize.getX(), 
                                  std::min(sphere->getCenter().getX(), aabbCenter.getX() + aabbHalfSize.getX())));
        closestPoint.setY(std::max(aabbCenter.getY() - aabbHalfSize.getY(), 
                                  std::min(sphere->getCenter().getY(), aabbCenter.getY() + aabbHalfSize.getY())));
        closestPoint.setZ(std::max(aabbCenter.getZ() - aabbHalfSize.getZ(), 
                                  std::min(sphere->getCenter().getZ(), aabbCenter.getZ() + aabbHalfSize.getZ())));
        
        // Check if closest point is inside sphere
        double distance = sphere->getCenter().distanceTo(closestPoint);
        return distance < sphere->getRadius();
    }
    return false;
}
