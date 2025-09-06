#ifndef GAMEENGINE_H
#define GAMEENGINE_H

#include "position.h"
#include "player_controller.h"
#include "physics_system.h"
#include "loot_system.h"
#include "dungeon.h"
#include "ps1_graphics_manager.h"
#include <string>
#include <vector>
#include <chrono>
#include <memory>
#include <map>

// Forward declarations
class Character;
class Mob;
class Ability;
class ProjectileManager;

struct ProjectileInstance {
    Position currentPos;
    Position velocity;          // Units per second
    float timeAlive;           // Time since spawn in seconds
    float maxLifetime;         // Max time before projectile expires
    const Ability* sourceAbility;
    Character* caster;
    bool isActive;
    float radius;              // Collision radius
    
    // Optional physics properties
    float gravity;             // Downward acceleration (units/s²)
    float drag;                // Air resistance coefficient
    
    ProjectileInstance(const Position& startPos, const Position& vel, const Ability* ability, 
                      Character* casterPtr, float lifetime = 10.0f, float collisionRadius = 0.5f)
        : currentPos(startPos), velocity(vel), timeAlive(0.0f), maxLifetime(lifetime),
          sourceAbility(ability), caster(casterPtr), isActive(true), radius(collisionRadius),
          gravity(0.0f), drag(0.0f) {}
};

class ProjectileManager {
private:
    std::vector<ProjectileInstance> activeProjectiles;
    
public:
    void spawnProjectile(const Ability& ability, Character& caster, const Position& direction);
    void updateProjectiles(float deltaTime, std::vector<Character>& characters, std::vector<Mob>& mobs);
    void checkCollisions(ProjectileInstance& projectile, std::vector<Character>& characters, std::vector<Mob>& mobs);
    void removeInactiveProjectiles();
    
    // Getters for debugging/rendering
    const std::vector<ProjectileInstance>& getActiveProjectiles() const { return activeProjectiles; }
    size_t getProjectileCount() const { return activeProjectiles.size(); }
    void clearAllProjectiles() { activeProjectiles.clear(); }
};

class GameEngine {
private:
    std::vector<Character> characters;
    std::vector<Mob> mobs;
    std::unique_ptr<ProjectileManager> projectileManager;
    std::unique_ptr<PlayerController> playerController;
    std::unique_ptr<PhysicsSystem> physicsSystem;
    std::shared_ptr<LootSystem> lootSystem;
    std::unique_ptr<LootDropManager> lootDropManager;
    std::unique_ptr<DungeonManager> dungeonManager;
    std::unique_ptr<PS1GraphicsManager> graphicsManager;
    
    // Physics body storage
    std::map<std::string, std::shared_ptr<PhysicsBody>> characterPhysicsBodies;
    std::map<std::string, std::shared_ptr<PhysicsBody>> mobPhysicsBodies;
    
    // Timing
    std::chrono::steady_clock::time_point lastUpdateTime;
    float targetFPS;
    float fixedDeltaTime;
    bool useFixedTimeStep;
    
    // Game state
    bool isRunning;
    bool isPaused;
    
public:
    GameEngine(float targetFPS = 60.0f, bool fixedTimeStep = false);
    ~GameEngine();
    
    // Core game loop methods
    void initialize();
    void run();
    void update(float deltaTime);
    void shutdown();
    
    // Entity management
    void addCharacter(const Character& character);
    void addMob(const Mob& mob);
    Character* getCharacter(const std::string& name);
    Mob* getMob(size_t index);
    
    // Projectile system access
    ProjectileManager& getProjectileManager() { return *projectileManager; }
    
    // Player controller access
    PlayerController& getPlayerController() { return *playerController; }
    
    // Physics system access
    PhysicsSystem& getPhysicsSystem() { return *physicsSystem; }
    
    // Loot system access
    LootSystem& getLootSystem() { return *lootSystem; }
    LootDropManager& getLootDropManager() { return *lootDropManager; }
    
    // Dungeon system access
    DungeonManager& getDungeonManager() { return *dungeonManager; }
    
    // Graphics system access
    PS1GraphicsManager& getGraphicsManager() { return *graphicsManager; }
    
    // Game state control
    void pause() { isPaused = true; }
    void resume() { isPaused = false; }
    void stop() { isRunning = false; }
    bool getIsRunning() const { return isRunning; }
    bool getIsPaused() const { return isPaused; }
    
    // Timing utilities
    float getDeltaTime();
    float getTargetFPS() const { return targetFPS; }
    void setTargetFPS(float fps) { targetFPS = fps; fixedDeltaTime = 1.0f / fps; }
    
    // Debug methods
    void printGameState() const;
    void printProjectileInfo() const;
    
    // Physics integration methods
    void syncPhysicsBodies();
    void updateEntityPhysics(float deltaTime);
    
    // Entity access methods
    Mob& getMob(const std::string& description);
    
    // Loot generation methods
    void generateMobLoot(const Mob& mob, const Position& dropPosition);
    void generateDungeonLoot(const std::string& dungeonTier, const Position& dropPosition, leveltype playerLevel);
};

#endif // GAMEENGINE_H
