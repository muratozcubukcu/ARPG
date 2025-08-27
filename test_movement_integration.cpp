#include "statuseffect.h"
#include "character.h"
#include "mob.h"
#include "race.h"
#include "class.h"
#include "position.h"
#include <iostream>

int main() {
    std::cout << "=== Movement System Integration Test ===" << std::endl;
    
    // Create a test character
    Race testRace = Race::createHuman();
    Class testClass = Class::createWarrior();
    Character player("TestPlayer", testRace, testClass);
    
    // Set initial position
    player.setPosition(0.0, 0.0, 0.0);
    std::cout << "Player starts at position: (" << player.getPosition().getX() 
              << ", " << player.getPosition().getY() << ", " << player.getPosition().getZ() << ")" << std::endl;
    
    // Test normal movement
    std::cout << "\n=== Normal Movement Test ===" << std::endl;
    std::cout << "Normal movement speed: " << player.getStats().getMovementSpeed() << std::endl;
    
    // Move normally
    player.move(5.0, 3.0, 1.0);
    std::cout << "After normal movement: (" << player.getPosition().getX() 
              << ", " << player.getPosition().getY() << ", " << player.getPosition().getZ() << ")" << std::endl;
    
    // Reset position
    player.setPosition(0.0, 0.0, 0.0);
    
    // Test ROOT effect on movement
    std::cout << "\n=== ROOT Effect Test ===" << std::endl;
    StatusEffect rootEffect("Root", "Cannot move", ROOT, 0, 10.0f, REFRESH);
    player.addStatusEffect(rootEffect);
    
    std::cout << "Player rooted: " << (player.getIsRooted() ? "YES" : "NO") << std::endl;
    
    // Try to move while rooted
    player.move(5.0, 3.0, 1.0);
    std::cout << "After movement attempt while rooted: (" << player.getPosition().getX() 
              << ", " << player.getPosition().getY() << ", " << player.getPosition().getZ() << ")" << std::endl;
    
    // Remove root effect
    player.removeStatusEffect("Root");
    std::cout << "Root removed. Player rooted: " << (player.getIsRooted() ? "YES" : "NO") << std::endl;
    
    // Test SLOW_MOVEMENT effect
    std::cout << "\n=== SLOW_MOVEMENT Effect Test ===" << std::endl;
    StatusEffect slowMoveEffect("Slow Movement", "Reduced movement speed", SLOW_MOVEMENT, 60, 8.0f, REFRESH);
    player.addStatusEffect(slowMoveEffect);
    
    std::cout << "Movement speed reduced to: " << player.getStats().getMovementSpeed() << std::endl;
    
    // Move with reduced speed
    player.move(5.0, 3.0, 1.0);
    std::cout << "After slow movement: (" << player.getPosition().getX() 
              << ", " << player.getPosition().getY() << ", " << player.getPosition().getZ() << ")" << std::endl;
    
    // Reset position
    player.setPosition(0.0, 0.0, 0.0);
    
    // Test SLOW_ATTACK effect
    std::cout << "\n=== SLOW_ATTACK Effect Test ===" << std::endl;
    StatusEffect slowAttackEffect("Slow Attack", "Reduced attack speed", SLOW_ATTACK, 50, 6.0f, REFRESH);
    player.addStatusEffect(slowAttackEffect);
    
    std::cout << "Attack speed reduced to: " << player.getStats().getAttackSpeed() << std::endl;
    
    // Test STUN effect
    std::cout << "\n=== STUN Effect Test ===" << std::endl;
    StatusEffect stunEffect("Stun", "Cannot act", STUN, 0, 5.0f, REFRESH);
    player.addStatusEffect(stunEffect);
    
    std::cout << "Player stunned: " << (player.getIsStunned() ? "YES" : "NO") << std::endl;
    
    // Try to move while stunned
    player.move(2.0, 1.0, 0.5);
    std::cout << "After movement attempt while stunned: (" << player.getPosition().getX() 
              << ", " << player.getPosition().getY() << ", " << player.getPosition().getZ() << ")" << std::endl;
    
    // Test status effect expiration
    std::cout << "\n=== Status Effect Expiration Test ===" << std::endl;
    std::cout << "Current status effects: " << player.getStatusEffects().size() << std::endl;
    
    // Simulate time passing to expire some effects
    float deltaTime = 1.0f;
    for (int i = 0; i < 7; i++) {
        player.updateStatusEffects(deltaTime);
        std::cout << "After " << (i + 1) << " second(s): " << player.getStatusEffects().size() << " effects remaining" << std::endl;
    }
    
    std::cout << "\nFinal status:" << std::endl;
    std::cout << "Player rooted: " << (player.getIsRooted() ? "YES" : "NO") << std::endl;
    std::cout << "Player stunned: " << (player.getIsStunned() ? "YES" : "NO") << std::endl;
    std::cout << "Movement speed: " << player.getStats().getMovementSpeed() << std::endl;
    std::cout << "Attack speed: " << player.getStats().getAttackSpeed() << std::endl;
    
    std::cout << "\n=== Test Complete ===" << std::endl;
    
    return 0;
}
