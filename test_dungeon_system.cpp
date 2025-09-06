#include "dungeon.h"
#include "character.h"
#include "race.h"
#include "class.h"
#include "loot_system.h"
#include <iostream>
#include <memory>

int main() {
    std::cout << "=== Testing Dungeon System ===" << std::endl;
    
    // Create loot system
    auto lootSystem = std::make_shared<LootSystem>();
    
    // Create dungeon manager
    DungeonManager dungeonManager(lootSystem);
    
    // Display available dungeons
    std::cout << "\n=== Available Dungeons ===" << std::endl;
    dungeonManager.printAllDungeons();
    
    // Create a test character
    Character player("TestPlayer", Race::createHuman(), Class::createWarrior());
    
    // Set player level to test different dungeon tiers
    std::cout << "\n=== Testing Dungeon Access ===" << std::endl;
    std::cout << "Player level: " << player.getStats().getLevel() << std::endl;
    
    // Test dungeon access at different levels
    for (int level = 1; level <= 50; level += 10) {
        player.getStatsRef().setLevel(level);
        std::cout << "\nPlayer level " << level << ":" << std::endl;
        
        auto availableDungeons = dungeonManager.getAvailableDungeons(level);
        for (const auto& dungeonName : availableDungeons) {
            std::cout << "  - " << dungeonName << std::endl;
        }
    }
    
    // Test entering a specific dungeon
    std::cout << "\n=== Testing Dungeon Entry ===" << std::endl;
    player.getStatsRef().setLevel(15); // Set to level 15 to access Tier 2 dungeons
    
    std::cout << "Attempting to enter 'Orc Stronghold'..." << std::endl;
    bool entered = dungeonManager.enterDungeon(player, "Orc Stronghold");
    
    if (entered) {
        std::cout << "Successfully entered dungeon!" << std::endl;
        std::cout << "Player position: " << player.getPosition() << std::endl;
        
        // Get dungeon info
        auto dungeon = dungeonManager.getDungeon("Orc Stronghold");
        if (dungeon) {
            std::cout << "\n=== Dungeon Information ===" << std::endl;
            std::cout << dungeon->getDescription();
            
            std::cout << "\n=== Dungeon Layout ===" << std::endl;
            dungeon->printLayout();
            
            // Test enemy spawning
            std::cout << "\n=== Enemy Information ===" << std::endl;
            const auto& enemies = dungeon->getEnemies();
            std::cout << "Total enemies: " << enemies.size() << std::endl;
            
            for (size_t i = 0; i < std::min(enemies.size(), size_t(3)); ++i) {
                const auto& enemy = enemies[i];
                std::cout << "Enemy " << i + 1 << ": " << enemy.getDescription() << std::endl;
                std::cout << "  Position: " << enemy.getPosition() << std::endl;
                std::cout << "  Health: " << enemy.getStats().getHealth() << "/" << enemy.getStats().getMaxHealth() << std::endl;
            }
            
            // Test loot information
            std::cout << "\n=== Loot Information ===" << std::endl;
            const auto& loot = dungeon->getLoot();
            std::cout << "Total loot items: " << loot.size() << std::endl;
            
            for (size_t i = 0; i < std::min(loot.size(), size_t(3)); ++i) {
                const auto& item = loot[i];
                std::cout << "Loot " << i + 1 << ": " << item.getName() << " (" << item.getRarityString() << ")" << std::endl;
                std::cout << "  Required Level: " << item.getRequiredLevel() << std::endl;
            }
            
            // Test boss information
            std::cout << "\n=== Boss Information ===" << std::endl;
            const auto& boss = dungeon->getBoss();
            std::cout << "Boss: " << boss.name << std::endl;
            std::cout << "Description: " << boss.description << std::endl;
            std::cout << "Level: " << boss.level << std::endl;
            std::cout << "Health: " << boss.stats.getHealth() << "/" << boss.stats.getMaxHealth() << std::endl;
            
            // Test exiting dungeon
            std::cout << "\n=== Testing Dungeon Exit ===" << std::endl;
            dungeon->exit(player);
            std::cout << "Player position after exit: " << player.getPosition() << std::endl;
        }
    } else {
        std::cout << "Failed to enter dungeon!" << std::endl;
    }
    
    // Test portal detection
    std::cout << "\n=== Testing Portal Detection ===" << std::endl;
    auto nearbyPortals = dungeonManager.getPortalsInRange(Position(100, 0, 100), 50.0);
    std::cout << "Portals near (100, 0, 100): " << nearbyPortals.size() << std::endl;
    
    for (const auto& portal : nearbyPortals) {
        std::cout << "  - " << portal.dungeonName << " at " << portal.worldPosition << std::endl;
    }
    
    // Test dungeon info for non-generated dungeons
    std::cout << "\n=== Testing Dungeon Info ===" << std::endl;
    std::cout << dungeonManager.getDungeonInfo("Goblin Mines");
    
    // Test dungeon reset
    std::cout << "\n=== Testing Dungeon Reset ===" << std::endl;
    auto dungeon = dungeonManager.getDungeon("Orc Stronghold");
    if (dungeon) {
        std::cout << "Before reset - Generated: " << (dungeon->isCompleted() ? "Yes" : "No") << std::endl;
        dungeon->reset();
        std::cout << "After reset - Generated: " << (dungeon->isCompleted() ? "Yes" : "No") << std::endl;
    }
    
    std::cout << "\n=== Dungeon System Test Complete ===" << std::endl;
    return 0;
}
