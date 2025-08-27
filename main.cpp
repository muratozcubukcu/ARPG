#include "character.h"
#include "mob.h"
#include "gameengine.h"
#include <iostream>
#include <thread>
#include <chrono>

int main() {
    std::cout << "=== RPG Game with Tick-Based Combat System ===" << std::endl;
    
    // Create game engine with 60 FPS target
    GameEngine engine(60.0f, true); // Use fixed timestep for consistent simulation
    
    // Create characters and mobs
    Character p1 = Character("Balthazar", Race::createHuman(), Class::createMage());
    Mob dragon = Mob(Race::createDragon());
    
    // Set initial positions
    p1.setPosition(10.0, 5.0, 0.0);
    dragon.setPosition(25.0, 15.0, 0.0);
    
    // Add entities to the game engine
    engine.addCharacter(p1);
    engine.addMob(dragon);
    
    // Display initial game state
    std::cout << "\n=== Initial Game State ===" << std::endl;
    engine.printGameState();
    
    // Get references to entities from the engine
    std::cout << "\n=== DEBUG: Retrieving entities from engine ===" << std::endl;
    Character* player = engine.getCharacter("Balthazar");
    Mob* dragonPtr = engine.getMob(0);
    
    // DIAGNOSTIC LOGGING
    std::cout << "DEBUG: player pointer = " << (player ? "VALID" : "NULL") << std::endl;
    std::cout << "DEBUG: dragonPtr = " << (dragonPtr ? "VALID" : "NULL") << std::endl;
    
    if (player) {
        auto abilities = player->getAbilities();
        std::cout << "DEBUG: player has " << abilities.size() << " abilities" << std::endl;
        if (!abilities.empty()) {
            std::cout << "DEBUG: first ability name = " << abilities[0].getName() << std::endl;
            std::cout << "DEBUG: first ability cast type = " << abilities[0].getCastType() << std::endl;
        }
    } else {
        std::cout << "ERROR: Failed to retrieve player 'Balthazar' from engine!" << std::endl;
    }
    
    if (!dragonPtr) {
        std::cout << "ERROR: Failed to retrieve dragon at index 0 from engine!" << std::endl;
    }
    
    std::cout << "DEBUG: Checking condition: player=" << (player ? "OK" : "FAIL") 
              << ", dragonPtr=" << (dragonPtr ? "OK" : "FAIL")
              << ", abilities=" << (player && !player->getAbilities().empty() ? "OK" : "FAIL") << std::endl;
    
    if (player && dragonPtr && !player->getAbilities().empty()) {
        std::cout << "\n=== Testing Ability System ===" << std::endl;
        
        std::cout << "DEBUG: Getting first ability reference..." << std::endl;
        Ability& firstAbility = const_cast<Ability&>(player->getAbilities()[0]);
        
        std::cout << "DEBUG: Getting ability name..." << std::endl;
        try {
            std::string abilityName = firstAbility.getName();
            std::cout << "DEBUG: Got ability name successfully: " << abilityName << std::endl;
            std::cout << "Testing ability: " << abilityName << std::endl;
        } catch (const std::exception& e) {
            std::cout << "EXCEPTION in getName(): " << e.what() << std::endl;
            return 1;
        } catch (...) {
            std::cout << "UNKNOWN EXCEPTION in getName()" << std::endl;
            return 1;
        }
        
        std::cout << "DEBUG: Getting ability range..." << std::endl;
        std::cout << "Ability range: " << firstAbility.getRange() << std::endl;
        
        std::cout << "DEBUG: Calculating distance to dragon..." << std::endl;
        std::cout << "Distance to dragon: " << player->getPosition().distanceTo(dragonPtr->getPosition()) << std::endl;
        
        std::cout << "DEBUG: Checking if ability is in range..." << std::endl;
        std::cout << "Can cast on dragon? " << (firstAbility.isInRange(player->getPosition(), dragonPtr->getPosition()) ? "Yes" : "No") << std::endl;
        
        std::cout << "DEBUG: Checking ability cast type..." << std::endl;
        // Configure ability for projectile casting if it isn't already
        if (firstAbility.getCastType() != PROJECTILE_CAST) {
            std::cout << "Converting ability to projectile type for demonstration..." << std::endl;
            // We'll need to modify the ability to be a projectile type
            // For now, let's create a new projectile ability
        }
        
        std::cout << "DEBUG: About to test different casting methods..." << std::endl;
        // Test different casting methods
        std::cout << "\n=== Testing Different Cast Types ===" << std::endl;
        
        // Test self cast (healing/buff)
        std::cout << "\n1. Testing Self Cast:" << std::endl;
        firstAbility.castSelf(*player);
        
        // Test projectile cast with the new system
        std::cout << "\n2. Testing NEW Tick-Based Projectile Cast:" << std::endl;
        
        // Move player closer for better demonstration
        std::cout << "Moving player closer to dragon..." << std::endl;
        player->move(5.0, 5.0, 0.0);
        std::cout << "Player new position: " << player->getPosition() << std::endl;
        
        // Calculate direction to dragon
        Position playerPos = player->getPosition();
        Position dragonPos = dragonPtr->getPosition();
        Position direction = (dragonPos - playerPos).normalize();
        
        std::cout << "Firing projectile from " << playerPos << " towards " << dragonPos << std::endl;
        std::cout << "Direction vector: " << direction << std::endl;
        
        // Create a projectile ability for demonstration
        Ability projectileSpell("Magic Missile", "A guided magical projectile", MAGICAL, 25, 10, 2, 1, 30, 
                               PROJECTILE, DAMAGE, ACTIVE, PROJECTILE_CAST, SINGLE_TARGET, 15.0f, 1.0f);
        
        // Cast the projectile using the new system
        projectileSpell.castProjectile(*player, direction, engine.getProjectileManager());
        
        // Test ground target cast
        std::cout << "\n3. Testing Ground Target Cast:" << std::endl;
        std::vector<Character> charTargets = {};
        std::vector<Mob> mobTargets = {*dragonPtr};
        firstAbility.castGroundTarget(*player, dragonPtr->getPosition(), charTargets, mobTargets);
        
        std::cout << "\n=== Starting Game Loop to Simulate Projectiles ===" << std::endl;
        engine.printProjectileInfo();
        
        // Run the game engine to simulate projectile movement
        engine.run();
        
        std::cout << "\n=== Final Game State ===" << std::endl;
        engine.printGameState();
        
        // Test new movement system
        std::cout << "\n=== Testing New Movement System ===" << std::endl;
        
        std::cout << "Starting movement test..." << std::endl;
        std::cout << "Initial player position: " << player->getPosition() << std::endl;
        
        // Get the player controller and test movement
        PlayerController& playerController = engine.getPlayerController();
        playerController.setPlayerCharacter(player);
        
        // Test WASD movement simulation
        std::cout << "\n--- Testing WASD movement ---" << std::endl;
        
        // Simulate pressing W (forward)
        playerController.moveForward(true);
        for (int i = 0; i < 30; i++) { // Run for 0.5 seconds
            playerController.update();
            engine.update(0.016f);
        }
        std::cout << "After W movement: " << player->getPosition() << std::endl;
        
        // Simulate pressing D (right)
        playerController.moveForward(false);
        playerController.moveRight(true);
        for (int i = 0; i < 30; i++) { // Run for 0.5 seconds
            playerController.update();
            engine.update(0.016f);
        }
        std::cout << "After D movement: " << player->getPosition() << std::endl;
        
        // Simulate pressing S (backward)
        playerController.moveRight(false);
        playerController.moveBackward(true);
        for (int i = 0; i < 30; i++) { // Run for 0.5 seconds
            playerController.update();
            engine.update(0.016f);
        }
        std::cout << "\nMovement test completed!" << std::endl;
        std::cout << "Final player position: " << player->getPosition() << std::endl;
    } else {
        std::cout << "\nERROR: Cannot proceed with ability testing!" << std::endl;
        std::cout << "Reason: ";
        if (!player) std::cout << "Player is null. ";
        if (!dragonPtr) std::cout << "Dragon is null. ";
        if (player && player->getAbilities().empty()) std::cout << "Player has no abilities. ";
        std::cout << std::endl;
    }
    
    std::cout << "\n=== Game Complete ===" << std::endl;
    return 0;
}