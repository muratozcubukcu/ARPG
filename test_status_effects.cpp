#include "statuseffect.h"
#include "character.h"
#include "mob.h"
#include "race.h"
#include "class.h"
#include <iostream>

int main() {
    std::cout << "=== Status Effects Test Program ===" << std::endl;
    
    // Create a test race and class
    Race testRace = Race::createHuman();
    Class testClass = Class::createWarrior();
    
    // Create a test character
    Character player("TestPlayer", testRace, testClass);
    std::cout << "Created character: " << player.getFullDescription() << std::endl;
    std::cout << "Initial movement speed: " << player.getStats().getMovementSpeed() << std::endl;
    std::cout << "Initial attack speed: " << player.getStats().getAttackSpeed() << std::endl;
    
    // Create a test mob
    Mob enemy(testRace);
    std::cout << "Created mob: " << enemy.getFullDescription() << std::endl;
    std::cout << "Initial movement speed: " << enemy.getStats().getMovementSpeed() << std::endl;
    std::cout << "Initial attack speed: " << enemy.getStats().getAttackSpeed() << std::endl;
    
    std::cout << "\n=== Testing Status Effects ===" << std::endl;
    
    // Test ROOT effect on character
    StatusEffect rootEffect("Root", "Cannot move", ROOT, 0, 5.0f, REFRESH);
    player.addStatusEffect(rootEffect);
    std::cout << "Player rooted: " << (player.getIsRooted() ? "YES" : "NO") << std::endl;
    
    // Test SLOW_MOVEMENT effect on character
    StatusEffect slowMoveEffect("Slow Movement", "Reduced movement speed", SLOW_MOVEMENT, 50, 8.0f, REFRESH);
    player.addStatusEffect(slowMoveEffect);
    std::cout << "Player movement speed: " << player.getStats().getMovementSpeed() << std::endl;
    
    // Test SLOW_ATTACK effect on character
    StatusEffect slowAttackEffect("Slow Attack", "Reduced attack speed", SLOW_ATTACK, 30, 6.0f, REFRESH);
    player.addStatusEffect(slowAttackEffect);
    std::cout << "Player attack speed: " << player.getStats().getAttackSpeed() << std::endl;
    
    // Test STUN effect on mob
    StatusEffect stunEffect("Stun", "Cannot act", STUN, 0, 4.0f, REFRESH);
    enemy.addStatusEffect(stunEffect);
    std::cout << "Enemy stunned: " << (enemy.getIsStunned() ? "YES" : "NO") << std::endl;
    
    // Test SLOW_MOVEMENT effect on mob
    StatusEffect mobSlowEffect("Mob Slow", "Reduced movement speed", SLOW_MOVEMENT, 60, 7.0f, REFRESH);
    enemy.addStatusEffect(mobSlowEffect);
    std::cout << "Enemy movement speed: " << enemy.getStats().getMovementSpeed() << std::endl;
    
    std::cout << "\n=== Testing Status Effect Updates ===" << std::endl;
    
    // Simulate time passing
    float deltaTime = 1.0f; // 1 second
    std::cout << "Updating status effects for " << deltaTime << " second..." << std::endl;
    
    player.updateStatusEffects(deltaTime);
    enemy.updateStatusEffects(deltaTime);
    
    std::cout << "Player status effects: " << player.getStatusEffects().size() << std::endl;
    std::cout << "Enemy status effects: " << enemy.getStatusEffects().size() << std::endl;
    
    // Simulate more time passing to expire some effects
    std::cout << "\nSimulating 6 seconds passing..." << std::endl;
    for (int i = 0; i < 6; i++) {
        player.updateStatusEffects(deltaTime);
        enemy.updateStatusEffects(deltaTime);
    }
    
    std::cout << "After 6 seconds:" << std::endl;
    std::cout << "Player status effects: " << player.getStatusEffects().size() << std::endl;
    std::cout << "Enemy status effects: " << enemy.getStatusEffects().size() << std::endl;
    std::cout << "Player rooted: " << (player.getIsRooted() ? "YES" : "NO") << std::endl;
    std::cout << "Enemy stunned: " << (enemy.getIsStunned() ? "YES" : "NO") << std::endl;
    std::cout << "Player movement speed: " << player.getStats().getMovementSpeed() << std::endl;
    std::cout << "Player attack speed: " << player.getStats().getAttackSpeed() << std::endl;
    std::cout << "Enemy movement speed: " << enemy.getStats().getMovementSpeed() << std::endl;
    
    std::cout << "\n=== Test Complete ===" << std::endl;
    
    return 0;
}
