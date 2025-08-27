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
    }
    
    // Update projectiles
    projectileManager->updateProjectiles(deltaTime, characters, mobs);
    
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
    projectileManager->clearAllProjectiles();
    characters.clear();
    mobs.clear();
    std::cout << "Game Engine shutdown complete." << std::endl;
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
    // TODO: Register with physics system
    std::cout << "Added character: " << character.getName() << std::endl;
}

void GameEngine::addMob(const Mob& mob) {
    mobs.push_back(mob);
    // TODO: Register with physics system
    std::cout << "Added mob: " << mob.getDescription() << std::endl;
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
