#include "gameengine.h"
#include "character.h"
#include "mob.h"
#include "race.h"
#include "class.h"
#include <iostream>

int main() {
    std::cout << "=== Physics System Integration Test ===" << std::endl;
    
    // Create game engine with physics enabled
    GameEngine engine(60.0f, true);
    
    // Create test characters and mobs
    Character player1 = Character("TestPlayer1", Race::createHuman(), Class::createWarrior());
    Character player2 = Character("TestPlayer2", Race::createHuman(), Class::createMage());
    Mob dragon = Mob(Race::createDragon());
    
    // Set initial positions - start higher to see gravity effect
    player1.setPosition(0.0, 0.0, 5.0);
    player2.setPosition(5.0, 0.0, 8.0);
    dragon.setPosition(10.0, 0.0, 3.0);
    
    std::cout << "\n=== Adding entities to physics system ===" << std::endl;
    
    // Add entities to the game engine (this will create physics bodies)
    engine.addCharacter(player1);
    engine.addCharacter(player2);
    engine.addMob(dragon);
    
    // Get pointers to the characters stored in the engine
    auto* enginePlayer1 = engine.getCharacter("TestPlayer1");
    auto* enginePlayer2 = engine.getCharacter("TestPlayer2");
    auto& engineDragon = engine.getMob("Dragon");
    
    std::cout << "\n=== Testing physics simulation ===" << std::endl;
    
    // Run physics simulation for a few frames
    float deltaTime = 0.016f; // 60 FPS
    for (int frame = 0; frame < 60; frame++) {
        engine.update(deltaTime);
        
        if (frame % 20 == 0) { // Print every 20 frames
            std::cout << "Frame " << frame << ":" << std::endl;
            std::cout << "  Player1: " << enginePlayer1->getPosition() << std::endl;
            std::cout << "  Player2: " << enginePlayer2->getPosition() << std::endl;
            std::cout << "  Dragon: " << engineDragon.getPosition() << std::endl;
            
            // Check if physics bodies are being updated
            auto& physicsSystem = engine.getPhysicsSystem();
            std::cout << "  Active physics bodies: " << physicsSystem.getBodyCount() << std::endl;
        }
    }
    
    std::cout << "\n=== Testing collision detection ===" << std::endl;
    
    // Move characters close to each other to test collision
    enginePlayer1->setPosition(0.0, 0.0, 1.0);
    enginePlayer2->setPosition(0.5, 0.0, 1.0); // Very close to player1
    
    std::cout << "Moving characters close together..." << std::endl;
    std::cout << "Player1: " << enginePlayer1->getPosition() << std::endl;
    std::cout << "Player2: " << enginePlayer2->getPosition() << std::endl;
    
    // Run physics for a few frames to resolve collision
    for (int frame = 0; frame < 30; frame++) {
        engine.update(deltaTime);
    }
    
    std::cout << "\nAfter collision resolution:" << std::endl;
    std::cout << "Player1: " << enginePlayer1->getPosition() << std::endl;
    std::cout << "Player2: " << enginePlayer2->getPosition() << std::endl;
    
    std::cout << "\n=== Testing gravity ===" << std::endl;
    
    // Move characters up to test gravity
    enginePlayer1->setPosition(0.0, 0.0, 10.0);
    enginePlayer2->setPosition(5.0, 0.0, 15.0);
    
    std::cout << "Moving characters up in the air..." << std::endl;
    std::cout << "Player1: " << enginePlayer1->getPosition() << std::endl;
    std::cout << "Player2: " << enginePlayer2->getPosition() << std::endl;
    
    // Run physics to see gravity effect
    for (int frame = 0; frame < 120; frame++) {
        engine.update(deltaTime);
        
        if (frame % 30 == 0) { // Print every 30 frames
            std::cout << "Frame " << frame << ":" << std::endl;
            std::cout << "  Player1: " << enginePlayer1->getPosition() << std::endl;
            std::cout << "  Player2: " << enginePlayer2->getPosition() << std::endl;
        }
    }
    
    std::cout << "\nAfter gravity simulation:" << std::endl;
    std::cout << "Player1: " << enginePlayer1->getPosition() << std::endl;
    std::cout << "Player2: " << enginePlayer2->getPosition() << std::endl;
    
    std::cout << "\n=== Physics Integration Test Complete ===" << std::endl;
    
    return 0;
}
