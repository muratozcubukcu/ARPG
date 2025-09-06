#include "loot_system.h"
#include "character.h"
#include "race.h"
#include "class.h"
#include <iostream>
#include <iomanip>

void printItem(const Item& item) {
    std::cout << "  " << std::left << std::setw(25) << item.getName();
    std::cout << " | " << std::setw(10) << item.getRarityString();
    std::cout << " | " << std::setw(8) << item.getTypeString();
    
    if (item.getType() == ItemType::WEAPON) {
        std::cout << " | Damage: " << std::setw(3) << item.getDamage();
    } else if (item.getType() == ItemType::ARMOR) {
        std::cout << " | Armor: " << std::setw(3) << item.getArmor();
    } else {
        std::cout << " | " << std::setw(8) << "";
    }
    
    std::cout << " | Level: " << std::setw(2) << item.getRequiredLevel();
    std::cout << " | Value: " << std::setw(3) << item.getGoldValue() << "g";
    std::cout << std::endl;
}

void printLootTable(const std::string& tableName, const std::vector<Item>& items) {
    std::cout << "\n=== " << tableName << " Loot Table ===" << std::endl;
    std::cout << std::left << std::setw(25) << "Item Name";
    std::cout << " | " << std::setw(10) << "Rarity";
    std::cout << " | " << std::setw(8) << "Type";
    std::cout << " | " << std::setw(8) << "Stats";
    std::cout << " | " << std::setw(8) << "Level";
    std::cout << " | " << std::setw(6) << "Value";
    std::cout << std::endl;
    std::cout << std::string(80, '-') << std::endl;
    
    for (const auto& item : items) {
        printItem(item);
    }
}

int main() {
    std::cout << "=== RPG Loot System Test ===" << std::endl;
    
    // Create loot system
    LootSystem lootSystem;
    
    // Set random seed for consistent testing
    lootSystem.setRandomSeed(42);
    
    std::cout << "\nAvailable loot tables: ";
    auto tables = lootSystem.getAvailableLootTables();
    for (const auto& table : tables) {
        std::cout << table << " ";
    }
    std::cout << std::endl;
    
    // Test class-specific equipment generation
    std::cout << "\n=== Testing Class-Specific Equipment Generation ===" << std::endl;
    
    std::vector<std::string> classes = {"Warrior", "Mage", "Archer", "Paladin"};
    std::vector<ItemRarity> rarities = {ItemRarity::COMMON, ItemRarity::UNCOMMON, ItemRarity::RARE, ItemRarity::EPIC, ItemRarity::LEGENDARY};
    
    for (const auto& className : classes) {
        std::cout << "\n--- " << className << " Equipment ---" << std::endl;
        
        for (const auto& rarity : rarities) {
            // Convert rarity to string for display
            std::string rarityStr;
            switch (rarity) {
                case ItemRarity::COMMON: rarityStr = "Common"; break;
                case ItemRarity::UNCOMMON: rarityStr = "Uncommon"; break;
                case ItemRarity::RARE: rarityStr = "Rare"; break;
                case ItemRarity::EPIC: rarityStr = "Epic"; break;
                case ItemRarity::LEGENDARY: rarityStr = "Legendary"; break;
                default: rarityStr = "Unknown"; break;
            }
            std::cout << "\n" << rarityStr << " rarity (Level 20):" << std::endl;
            
            // Generate class-specific loot
            auto classLoot = lootSystem.generateClassSpecificLoot(className, 20, rarity);
            
            if (classLoot.empty()) {
                std::cout << "  No equipment generated for this rarity/level combination." << std::endl;
            } else {
                for (const auto& item : classLoot) {
                    printItem(item);
                }
            }
        }
    }
    
    // Test mob loot generation
    std::cout << "\n=== Testing Mob Loot Generation ===" << std::endl;
    
    std::vector<std::string> mobTypes = {"Goblin", "Elite Orc", "Boss Dragon"};
    std::vector<leveltype> mobLevels = {5, 15, 30};
    
    for (size_t i = 0; i < mobTypes.size(); i++) {
        std::cout << "\n--- " << mobTypes[i] << " (Level " << mobLevels[i] << ") ---" << std::endl;
        
        auto mobLoot = lootSystem.generateMobLoot(mobTypes[i], mobLevels[i]);
        
        if (mobLoot.empty()) {
            std::cout << "  No loot dropped." << std::endl;
        } else {
            std::cout << "  Dropped " << mobLoot.size() << " items:" << std::endl;
            for (const auto& item : mobLoot) {
                printItem(item);
            }
        }
    }
    
    // Test dungeon loot generation
    std::cout << "\n=== Testing Dungeon Loot Generation ===" << std::endl;
    
    std::vector<std::string> dungeonTiers = {"Tier1", "Tier2", "Tier3"};
    std::vector<leveltype> playerLevels = {10, 25, 40};
    
    for (size_t i = 0; i < dungeonTiers.size(); i++) {
        std::cout << "\n--- " << dungeonTiers[i] << " Dungeon (Player Level " << playerLevels[i] << ") ---" << std::endl;
        
        auto dungeonLoot = lootSystem.generateDungeonLoot(dungeonTiers[i], playerLevels[i]);
        
        std::cout << "  Generated " << dungeonLoot.size() << " items:" << std::endl;
        for (const auto& item : dungeonLoot) {
            printItem(item);
        }
    }
    
    // Test rarity generation
    std::cout << "\n=== Testing Rarity Generation ===" << std::endl;
    
    std::vector<leveltype> testLevels = {1, 10, 25, 50, 100};
    
    for (const auto& level : testLevels) {
        auto rarity = lootSystem.generateRarity(level);
        // Convert rarity to string for display
        std::string rarityStr;
        switch (rarity) {
            case ItemRarity::COMMON: rarityStr = "Common"; break;
            case ItemRarity::UNCOMMON: rarityStr = "Uncommon"; break;
            case ItemRarity::RARE: rarityStr = "Rare"; break;
            case ItemRarity::EPIC: rarityStr = "Epic"; break;
            case ItemRarity::LEGENDARY: rarityStr = "Legendary"; break;
            default: rarityStr = "Unknown"; break;
        }
        std::cout << "Level " << std::setw(3) << level << ": " << rarityStr << std::endl;
    }
    
    // Test with luck bonus
    std::cout << "\n--- Testing with Luck Bonus ---" << std::endl;
    for (const auto& level : testLevels) {
        auto rarity = lootSystem.generateRarity(level, 0.2f); // 20% luck bonus
        // Convert rarity to string for display
        std::string rarityStr;
        switch (rarity) {
            case ItemRarity::COMMON: rarityStr = "Common"; break;
            case ItemRarity::UNCOMMON: rarityStr = "Uncommon"; break;
            case ItemRarity::RARE: rarityStr = "Rare"; break;
            case ItemRarity::EPIC: rarityStr = "Epic"; break;
            case ItemRarity::LEGENDARY: rarityStr = "Legendary"; break;
            default: rarityStr = "Unknown"; break;
        }
        std::cout << "Level " << std::setw(3) << level << " (with luck): " << rarityStr << std::endl;
    }
    
    // Test loot drop manager
    std::cout << "\n=== Testing Loot Drop Manager ===" << std::endl;
    
    LootDropManager dropManager(30.0f); // 30 second cleanup interval
    
    // Create some test items
    std::vector<Item> testLoot = {
        Item::createSword("Test Sword", ItemRarity::RARE),
        Item::createPotion("Test Potion", 50, 0)
    };
    
    Position dropPosition(10.0, 5.0, 0.0);
    
    // Add loot drop
    dropManager.addLootDrop(testLoot, dropPosition, 300.0f, false); // 5 minutes TTL
    
    std::cout << "Added loot drop at position " << dropPosition << std::endl;
    std::cout << "Active drops: " << dropManager.getActiveDropCount() << std::endl;
    
    // Simulate time passing
    std::cout << "Simulating 60 seconds passing..." << std::endl;
    dropManager.update(60.0f);
    
    // Check drops in area
    Position playerPos(12.0, 5.0, 0.0);
    auto nearbyDrops = dropManager.getDropsInRadius(playerPos, 5.0f);
    
    std::cout << "Drops near player: " << nearbyDrops.size() << std::endl;
    
    // Test permanent drops (quest items)
    std::vector<Item> questItems = {Item::createMaterial("Ancient Key", 1)};
    dropManager.addLootDrop(questItems, Position(20.0, 10.0, 0.0), 0.0f, true); // Permanent
    
    std::cout << "Added permanent quest item drop" << std::endl;
    std::cout << "Total active drops: " << dropManager.getActiveDropCount() << std::endl;
    
    std::cout << "\n=== Loot System Test Complete ===" << std::endl;
    
    return 0;
}
