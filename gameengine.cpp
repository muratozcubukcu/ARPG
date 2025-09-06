#include "gameengine.h"
#include "character.h"
#include "mob.h"
#include "ability.h"
#include <iostream>
#include <algorithm>
#include <thread>
#include <cmath>

// ProjectileManager Implementation
void ProjectileManager::spawnProjectile(const Ability& ability, Character& caster, const Position& direction) {
    Position startPos = caster.getPosition();
    
    // Calculate velocity based on projectile speed and direction
    Position normalizedDirection = direction.normalize();
    Position velocity = normalizedDirection * ability.getProjectileSpeed();
    
    // Calculate max lifetime based on range and speed
    float maxLifetime = ability.getRangeAsDouble() / ability.getProjectileSpeed();
    
    // Create and add projectile
    ProjectileInstance projectile(startPos, velocity, &ability, &caster, maxLifetime);
    
    // Apply some physics properties based on ability type
    if (ability.getType() == PHYSICAL) {
        projectile.gravity = 9.8f; // Physical projectiles affected by gravity
        projectile.drag = 0.1f;
    } else if (ability.getType() == MAGICAL) {
        projectile.gravity = 0.0f; // Magic projectiles ignore gravity
        projectile.drag = 0.0f;
    }
    
    activeProjectiles.push_back(projectile);
    
    std::cout << caster.getName() << " fires " << ability.getName() 
              << " projectile at speed " << ability.getProjectileSpeed() 
              << " for " << maxLifetime << " seconds!" << std::endl;
}

void ProjectileManager::updateProjectiles(float deltaTime, std::vector<Character>& characters, std::vector<Mob>& mobs) {
    for (auto& projectile : activeProjectiles) {
        if (!projectile.isActive) continue;
        
        // Update time alive
        projectile.timeAlive += deltaTime;
        
        // Check if projectile has expired
        if (projectile.timeAlive >= projectile.maxLifetime) {
            projectile.isActive = false;
            continue;
        }
        
        // Apply physics
        Position acceleration(0.0, 0.0, -projectile.gravity); // Gravity pulls down (negative Z)
        
        // Apply drag (simple linear drag)
        if (projectile.drag > 0.0f) {
            Position dragForce = projectile.velocity * (-projectile.drag);
            acceleration = acceleration + dragForce;
        }
        
        // Update velocity and position
        projectile.velocity = projectile.velocity + acceleration * deltaTime;
        projectile.currentPos = projectile.currentPos + projectile.velocity * deltaTime;
        
        // Check for ground collision (simple ground at z = 0)
        if (projectile.currentPos.getZ() <= 0.0 && projectile.velocity.getZ() < 0.0) {
            projectile.isActive = false;
            std::cout << "Projectile " << projectile.sourceAbility->getName() << " hits the ground!" << std::endl;
            continue;
        }
        
        // Check for entity collisions
        checkCollisions(projectile, characters, mobs);
    }
    
    // Remove inactive projectiles
    removeInactiveProjectiles();
}

void ProjectileManager::checkCollisions(ProjectileInstance& projectile, std::vector<Character>& characters, std::vector<Mob>& mobs) {
    if (!projectile.isActive) return;
    
    bool hitTarget = false;
    
    // Check character collisions
    for (auto& character : characters) {
        // Don't hit the caster
        if (&character == projectile.caster) continue;
        
        bool isHit = false;
        
        // Check collision based on ability shape
        switch (projectile.sourceAbility->getShape()) {
            case SINGLE_TARGET:
            case CIRCLE:
            case SPHERE: {
                double distance = projectile.currentPos.distanceTo(character.getPosition());
                isHit = (distance <= projectile.radius + 1.0); // 1.0 is character collision radius
                break;
            }
            case CONE: {
                // For cone projectiles, check if character is in the cone area
                double distance = projectile.currentPos.distanceTo(character.getPosition());
                if (distance <= projectile.sourceAbility->getEffectRadius() + 1.0) {
                    Position direction = projectile.velocity.normalize();
                    isHit = projectile.sourceAbility->isTargetInCone(projectile.currentPos, character.getPosition(), direction, 45.0f);
                }
                break;
            }
            case LINE: {
                Position direction = projectile.velocity.normalize();
                isHit = projectile.sourceAbility->isTargetInLine(projectile.currentPos, character.getPosition(), direction, projectile.sourceAbility->getEffectRadius());
                break;
            }
            default:
                double distance = projectile.currentPos.distanceTo(character.getPosition());
                isHit = (distance <= projectile.radius + 1.0);
                break;
        }
        
        if (isHit) {
            // Apply damage/effect
            if (projectile.sourceAbility->getEffect() == DAMAGE) {
                welltype damage = projectile.sourceAbility->calculateDamage(
                    projectile.caster->getStats().getStrength(),
                    projectile.caster->getStats().getIntelligence()
                );
                character.damage(damage);
                std::cout << "Projectile " << projectile.sourceAbility->getName() 
                          << " hits " << character.getName() << " for " << damage << " damage!" << std::endl;
            }
            hitTarget = true;
            
            // Only break for single-target projectiles
            if (projectile.sourceAbility->getShape() == SINGLE_TARGET) {
                break;
            }
        }
    }
    
    // Check mob collisions (allow multi-target projectiles to hit both characters and mobs)
    for (auto& mob : mobs) {
        bool isHit = false;
        
        // Check collision based on ability shape
        switch (projectile.sourceAbility->getShape()) {
            case SINGLE_TARGET:
            case CIRCLE:
            case SPHERE: {
                double distance = projectile.currentPos.distanceTo(mob.getPosition());
                isHit = (distance <= projectile.radius + 1.0); // 1.0 is mob collision radius
                break;
            }
            case CONE: {
                // For cone projectiles, check if mob is in the cone area
                double distance = projectile.currentPos.distanceTo(mob.getPosition());
                if (distance <= projectile.sourceAbility->getEffectRadius() + 1.0) {
                    Position direction = projectile.velocity.normalize();
                    isHit = projectile.sourceAbility->isTargetInCone(projectile.currentPos, mob.getPosition(), direction, 45.0f);
                }
                break;
            }
            case LINE: {
                Position direction = projectile.velocity.normalize();
                isHit = projectile.sourceAbility->isTargetInLine(projectile.currentPos, mob.getPosition(), direction, projectile.sourceAbility->getEffectRadius());
                break;
            }
            default:
                double distance = projectile.currentPos.distanceTo(mob.getPosition());
                isHit = (distance <= projectile.radius + 1.0);
                break;
        }
        
        if (isHit) {
            // Apply damage/effect
            if (projectile.sourceAbility->getEffect() == DAMAGE) {
                welltype damage = projectile.sourceAbility->calculateDamage(
                    projectile.caster->getStats().getStrength(),
                    projectile.caster->getStats().getIntelligence()
                );
                mob.damage(damage);
                std::cout << "Projectile " << projectile.sourceAbility->getName() 
                          << " hits " << mob.getDescription() << " for " << damage << " damage!" << std::endl;
            }
            hitTarget = true;
            
            // Only break for single-target projectiles
            if (projectile.sourceAbility->getShape() == SINGLE_TARGET) {
                break;
            }
        }
    }
    
    // Deactivate projectile if it hit something
    if (hitTarget) {
        projectile.isActive = false;
    }
}

void ProjectileManager::removeInactiveProjectiles() {
    activeProjectiles.erase(
        std::remove_if(activeProjectiles.begin(), activeProjectiles.end(),
            [](const ProjectileInstance& p) { return !p.isActive; }),
        activeProjectiles.end()
    );
}

// GameEngine Implementation
GameEngine::GameEngine(float targetFPS, bool fixedTimeStep) 
    : targetFPS(targetFPS), fixedDeltaTime(1.0f / targetFPS), useFixedTimeStep(fixedTimeStep),
      isRunning(false), isPaused(false) {
    projectileManager = std::make_unique<ProjectileManager>();
    playerController = std::make_unique<PlayerController>();
    physicsSystem = std::make_unique<PhysicsSystem>();
    lootSystem = std::make_unique<LootSystem>();
    lootDropManager = std::make_unique<LootDropManager>();
    dungeonManager = std::make_unique<DungeonManager>(lootSystem);
    graphicsManager = std::make_unique<PS1GraphicsManager>();
    lastUpdateTime = std::chrono::steady_clock::now();
}

GameEngine::~GameEngine() {
    shutdown();
}

void GameEngine::initialize() {
    std::cout << "Game Engine initialized with target FPS: " << targetFPS << std::endl;
    std::cout << "Fixed timestep: " << (useFixedTimeStep ? "ON" : "OFF") << std::endl;
    
    // Initialize systems
    // TODO: Initialize player controller and physics system
    
    // Initialize graphics system
    if (graphicsManager) {
        if (graphicsManager->initialize()) {
            graphicsManager->set_game_engine(this);
            std::cout << "PS1 Graphics Manager initialized successfully" << std::endl;
        } else {
            std::cerr << "Failed to initialize PS1 Graphics Manager" << std::endl;
        }
    }
    
    isRunning = true;
    isPaused = false;
    lastUpdateTime = std::chrono::steady_clock::now();
}

void GameEngine::run() {
    if (!isRunning) {
        initialize();
    }
    
    std::cout << "\n=== Starting Game Loop ===" << std::endl;
    std::cout << "Game is running. Type any key and press Enter to stop..." << std::endl;
    
    // Simple game loop for demonstration
    int frameCount = 0;
    while (isRunning && frameCount < 300) { // Run for ~5 seconds at 60 FPS
        if (!isPaused) {
            float deltaTime = useFixedTimeStep ? fixedDeltaTime : getDeltaTime();
            update(deltaTime);
        }
        
        frameCount++;
        
        // Simple frame rate limiting (simplified for now)
        // TODO: Add proper frame rate limiting when needed
    }
    
    std::cout << "\n=== Game Loop Ended ===" << std::endl;
    std::cout << "Total frames processed: " << frameCount << std::endl;
}

void GameEngine::update(float deltaTime) {
    // Update all game systems
    
    // Debug output removed for clean testing
    
    // Update player controller
    if (playerController) {
        playerController->update();
    }
    
    // Update physics system
    if (physicsSystem) {
        physicsSystem->update(deltaTime);
        updateEntityPhysics(deltaTime);
        syncPhysicsBodies();
    }
    
    // Update projectiles
    projectileManager->updateProjectiles(deltaTime, characters, mobs);
    
    // Update loot drop manager
    if (lootDropManager) {
        lootDropManager->update(deltaTime);
    }
    
    // Update graphics system
    if (graphicsManager) {
        graphicsManager->update_from_game_state();
        graphicsManager->render_frame();
    }
    
    // Update status effects for all entities
    for (auto& character : characters) {
        character.updateStatusEffects(deltaTime);
    }
    for (auto& mob : mobs) {
        // Debug output removed for clean testing
        mob.updateStatusEffects(deltaTime);
    }
    
    // Here you could add other systems:
    // - Character AI updates
    // - Mob movement
    // - Ability cooldowns
    // - Physics simulation
    
    // Debug output every 60 frames (roughly every second at 60 FPS)
    static int debugCounter = 0;
    debugCounter++;
    if (debugCounter >= 60) {
        if (projectileManager->getProjectileCount() > 0) {
            std::cout << "Active projectiles: " << projectileManager->getProjectileCount() << std::endl;
        }
        debugCounter = 0;
    }
}

void GameEngine::shutdown() {
    isRunning = false;
    // TODO: Shutdown player controller and physics system
    
    // Shutdown graphics system
    if (graphicsManager) {
        graphicsManager->shutdown();
    }
    
    projectileManager->clearAllProjectiles();
    characters.clear();
    mobs.clear();
    std::cout << "Game Engine shutdown complete." << std::endl;
}



Mob& GameEngine::getMob(const std::string& description) {
    for (auto& mob : mobs) {
        if (mob.getDescription() == description) {
            return mob;
        }
    }
    throw std::runtime_error("Mob not found: " + description);
}

float GameEngine::getDeltaTime() {
    auto currentTime = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(currentTime - lastUpdateTime);
    lastUpdateTime = currentTime;
    
    float deltaTime = duration.count() / 1000000.0f; // Convert to seconds
    
    // Cap delta time to prevent large jumps (e.g., when debugging)
    const float maxDeltaTime = 1.0f / 15.0f; // Cap at 15 FPS minimum
    return std::min(deltaTime, maxDeltaTime);
}

void GameEngine::addCharacter(const Character& character) {
    characters.push_back(character);
    
    // Register with physics system
    if (physicsSystem) {
        auto physicsBody = physicsSystem->createBody(character.getPosition(), 70.0f); // 70kg human
        physicsBody->bodyType = BodyType::DYNAMIC;
        
        // Initialize physics body with character position
        physicsBody->position = character.getPosition();
        physicsBody->velocity = Position(0, 0, 0); // Start with no velocity
        
        // Create appropriate collider based on character size
        auto collider = std::make_shared<SphereCollider>(character.getPosition(), 0.5f); // 0.5m radius
        physicsSystem->setBodyCollider(physicsBody, collider);
        
        // Store physics body reference in character (we'll need to add this to Character class)
        // For now, we'll store it in a map
        characterPhysicsBodies[character.getName()] = physicsBody;
        
        std::cout << "Added character: " << character.getName() << " with physics body at " << character.getPosition() << std::endl;
    } else {
        std::cout << "Added character: " << character.getName() << " (physics system not available)" << std::endl;
    }
}

void GameEngine::addMob(const Mob& mob) {
    mobs.push_back(mob);
    
    // Register with physics system
    if (physicsSystem) {
        float mass = 100.0f; // Default mass for mobs
        if (mob.getRace().getName() == "Dragon") {
            mass = 500.0f; // Dragons are heavy
        }
        
        auto physicsBody = physicsSystem->createBody(mob.getPosition(), mass);
        physicsBody->bodyType = BodyType::DYNAMIC;
        
        // Initialize physics body with mob position
        physicsBody->position = mob.getPosition();
        physicsBody->velocity = Position(0, 0, 0); // Start with no velocity
        
        // Create appropriate collider
        float radius = 1.0f; // Default radius
        if (mob.getRace().getName() == "Dragon") {
            radius = 2.0f; // Dragons are large
        }
        
        auto collider = std::make_shared<SphereCollider>(mob.getPosition(), radius);
        physicsSystem->setBodyCollider(physicsBody, collider);
        
        // Store physics body reference
        mobPhysicsBodies[mob.getDescription()] = physicsBody;
        
        std::cout << "Added mob: " << mob.getDescription() << " with physics body at " << mob.getPosition() << std::endl;
    } else {
        std::cout << "Added mob: " << mob.getDescription() << " (physics system not available)" << std::endl;
    }
}

Character* GameEngine::getCharacter(const std::string& name) {
    for (auto& character : characters) {
        if (character.getName() == name) {
            return &character;
        }
    }
    return nullptr;
}

Mob* GameEngine::getMob(size_t index) {
    if (index < mobs.size()) {
        return &mobs[index];
    }
    return nullptr;
}

void GameEngine::printGameState() const {
    std::cout << "\n=== Game State ===" << std::endl;
    std::cout << "Characters: " << characters.size() << std::endl;
    for (const auto& character : characters) {
        std::cout << "  - " << character.getName() << " at " << character.getPosition() << std::endl;
    }
    
    std::cout << "Mobs: " << mobs.size() << std::endl;
    for (const auto& mob : mobs) {
        std::cout << "  - " << mob.getDescription() << " at " << mob.getPosition() << std::endl;
    }
    
    std::cout << "Active Projectiles: " << projectileManager->getProjectileCount() << std::endl;
}

void GameEngine::printProjectileInfo() const {
    const auto& projectiles = projectileManager->getActiveProjectiles();
    std::cout << "\n=== Projectile Info ===" << std::endl;
    for (size_t i = 0; i < projectiles.size(); ++i) {
        const auto& p = projectiles[i];
        std::cout << "Projectile " << i << ": " << p.sourceAbility->getName() 
                  << " at " << p.currentPos 
                  << " (alive: " << p.timeAlive << "s)" << std::endl;
    }
}

// Physics integration methods implementation
void GameEngine::syncPhysicsBodies() {
    // Sync character positions with physics bodies
    for (auto& character : characters) {
        auto it = characterPhysicsBodies.find(character.getName());
        if (it != characterPhysicsBodies.end()) {
            auto& physicsBody = it->second;
            if (physicsBody && physicsBody->isActive) {
                // Update character position from physics body
                Position newPos = physicsBody->position;
                character.setPosition(newPos.getX(), newPos.getY(), newPos.getZ());
                
                // Debug output for gravity testing
                static int debugCounter = 0;
                if (debugCounter++ % 60 == 0) { // Every 60 frames
                    std::cout << "Character " << character.getName() 
                              << " physics pos: " << newPos 
                              << " velocity: " << physicsBody->velocity << std::endl;
                }
            }
        }
    }
    
    // Sync mob positions with physics bodies
    for (auto& mob : mobs) {
        auto it = mobPhysicsBodies.find(mob.getDescription());
        if (it != mobPhysicsBodies.end()) {
            auto& physicsBody = it->second;
            if (physicsBody && physicsBody->isActive) {
                // Update mob position from physics body
                Position newPos = physicsBody->position;
                mob.setPosition(newPos.getX(), newPos.getY(), newPos.getZ());
            }
        }
    }
}

void GameEngine::updateEntityPhysics(float deltaTime) {
    // Update character physics bodies - only apply forces, don't overwrite positions
    for (auto& character : characters) {
        auto it = characterPhysicsBodies.find(character.getName());
        if (it != characterPhysicsBodies.end()) {
            auto& physicsBody = it->second;
            if (physicsBody && physicsBody->isActive) {
                // Apply gravity force (always active)
                physicsBody->force.setZ(physicsBody->force.getZ() - 9.81f * physicsBody->mass);
                
                // Apply character movement as additional force if moving
                if (character.getStats().getMovementSpeed() > 0) {
                    // This will be enhanced later for proper movement physics
                    // For now, just apply gravity
                }
            }
        }
    }
    
    // Update mob physics bodies - only apply forces, don't overwrite positions
    for (auto& mob : mobs) {
        auto it = mobPhysicsBodies.find(mob.getDescription());
        if (it != mobPhysicsBodies.end()) {
            auto& physicsBody = it->second;
            if (physicsBody && physicsBody->isActive) {
                // Apply gravity to mobs
                physicsBody->force.setZ(physicsBody->force.getZ() - 9.81f * physicsBody->mass);
            }
        }
    }
}

// Loot generation methods implementation
void GameEngine::generateMobLoot(const Mob& mob, const Position& dropPosition) {
    if (!lootSystem || !lootDropManager) return;
    
    // Generate loot based on mob type and level
    auto loot = lootSystem->generateMobLoot(mob);
    
    if (!loot.empty()) {
        // Add loot drop to the world
        lootDropManager->addLootDrop(loot, dropPosition, 300.0f, false); // 5 minutes TTL
        
        std::cout << "Generated " << loot.size() << " loot items from " << mob.getDescription() << std::endl;
        for (const auto& item : loot) {
            std::cout << "  - " << item.getName() << " (" << item.getRarityString() << ")" << std::endl;
        }
    }
}

void GameEngine::generateDungeonLoot(const std::string& dungeonTier, const Position& dropPosition, leveltype playerLevel) {
    if (!lootSystem || !lootDropManager) return;
    
    // Generate dungeon-specific loot
    auto loot = lootSystem->generateDungeonLoot(dungeonTier, playerLevel);
    
    if (!loot.empty()) {
        // Add loot drop to the world
        lootDropManager->addLootDrop(loot, dropPosition, 600.0f, false); // 10 minutes TTL for dungeon loot
        
        std::cout << "Generated " << loot.size() << " dungeon loot items from " << dungeonTier << std::endl;
        for (const auto& item : loot) {
            std::cout << "  - " << item.getName() << " (" << item.getRarityString() << ")" << std::endl;
        }
    }
}
