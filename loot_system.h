#ifndef LOOT_SYSTEM_H
#define LOOT_SYSTEM_H

#include "item.h"
#include "class.h"
#include "types.h"
#include <vector>
#include <map>
#include <random>
#include <memory>

// Forward declarations
class Character;
class Mob;

// Loot table entry structure
struct LootEntry {
    Item item;
    float dropChance;        // 0.0 to 1.0 (percentage)
    stattype minQuantity;    // Minimum quantity to drop
    stattype maxQuantity;    // Maximum quantity to drop
    leveltype minLevel;      // Minimum level required to drop
    leveltype maxLevel;      // Maximum level for this loot tier
    
    LootEntry(const Item& item, float chance, stattype minQty = 1, stattype maxQty = 1, 
              leveltype minLvl = 1, leveltype maxLvl = 100)
        : item(item), dropChance(chance), minQuantity(minQty), maxQuantity(maxQty),
          minLevel(minLvl), maxLevel(maxLvl) {}
};

// Loot table for different mob types and dungeon tiers
class LootTable {
private:
    std::string tableName;
    std::vector<LootEntry> entries;
    float guaranteedDropChance;  // Chance to get at least one item
    stattype maxDrops;          // Maximum number of items that can drop
    
public:
    LootTable(const std::string& name, float guaranteedChance = 0.8f, stattype maxItems = 3);
    
    // Add loot entries
    void addEntry(const LootEntry& entry);
    void addEntry(const Item& item, float chance, stattype minQty = 1, stattype maxQty = 1,
                  leveltype minLvl = 1, leveltype maxLvl = 100);
    
    // Generate loot drops
    std::vector<Item> generateLoot(leveltype mobLevel, std::mt19937& rng) const;
    
    // Getters
    const std::string& getName() const { return tableName; }
    size_t getEntryCount() const { return entries.size(); }
    
    // Clear all entries
    void clear() { entries.clear(); }
};

// Class-specific equipment generator
class ClassEquipmentGenerator {
private:
    // Equipment templates for each class
    std::map<std::string, std::vector<Item>> warriorEquipment;
    std::map<std::string, std::vector<Item>> mageEquipment;
    std::map<std::string, std::vector<Item>> archerEquipment;
    std::map<std::string, std::vector<Item>> paladinEquipment;
    
    // Initialize equipment templates
    void initializeWarriorEquipment();
    void initializeMageEquipment();
    void initializeArcherEquipment();
    void initializePaladinEquipment();
    
public:
    ClassEquipmentGenerator();
    
    // Generate class-specific equipment
    Item generateClassWeapon(const std::string& className, ItemRarity rarity, leveltype level);
    Item generateClassArmor(const std::string& className, ArmorType armorType, ItemRarity rarity, leveltype level);
    
    // Get available equipment for a class
    std::vector<Item> getClassWeapons(const std::string& className, leveltype level);
    std::vector<Item> getClassArmor(const std::string& className, leveltype level);
    
    // Check if item is suitable for class
    bool isItemSuitableForClass(const Item& item, const std::string& className);
    
private:
    // Calculate rarity multiplier for stats
    float getRarityMultiplier(ItemRarity rarity);
};

// Main loot system manager
class LootSystem {
private:
    std::map<std::string, std::shared_ptr<LootTable>> lootTables;
    std::unique_ptr<ClassEquipmentGenerator> classEquipmentGen;
    std::mt19937 randomGenerator;
    
    // Predefined loot tables
    void initializeDefaultLootTables();
    
public:
    LootSystem();
    
    // Loot table management
    void addLootTable(const std::string& name, std::shared_ptr<LootTable> table);
    std::shared_ptr<LootTable> getLootTable(const std::string& name);
    
    // Generate loot from mobs
    std::vector<Item> generateMobLoot(const Mob& mob);
    std::vector<Item> generateMobLoot(const std::string& mobType, leveltype level);
    
    // Generate dungeon loot
    std::vector<Item> generateDungeonLoot(const std::string& dungeonTier, leveltype playerLevel);
    
    // Generate class-specific loot
    std::vector<Item> generateClassSpecificLoot(const std::string& className, leveltype level, ItemRarity rarity);
    
    // Utility methods
    void setRandomSeed(unsigned int seed);
    ItemRarity generateRarity(leveltype level, float luckBonus = 0.0f);
    
    // Get available loot tables
    std::vector<std::string> getAvailableLootTables() const;
    
    // Clear all loot tables
    void clearAllTables() { lootTables.clear(); }
};

// Loot drop event structure
struct LootDropEvent {
    std::vector<Item> items;
    Position dropPosition;
    float timeToLive;        // How long loot stays on ground
    bool isPermanent;        // Some loot (like quest items) never disappears
    
    LootDropEvent(const std::vector<Item>& items, const Position& pos, float ttl = 300.0f, bool permanent = false)
        : items(items), dropPosition(pos), timeToLive(ttl), isPermanent(permanent) {}
};

// Loot drop manager for handling dropped items in the world
class LootDropManager {
private:
    std::vector<LootDropEvent> activeDrops;
    float cleanupInterval;
    float lastCleanupTime;
    
public:
    LootDropManager(float cleanupInterval = 60.0f);
    
    // Add new loot drops
    void addLootDrop(const std::vector<Item>& items, const Position& position, float ttl = 300.0f, bool permanent = false);
    
    // Update and cleanup expired drops
    void update(float deltaTime);
    
    // Get loot drops in area
    std::vector<LootDropEvent> getDropsInRadius(const Position& center, float radius);
    
    // Remove specific loot drop
    void removeLootDrop(size_t index);
    
    // Clear all drops
    void clearAllDrops() { activeDrops.clear(); }
    
    // Get active drop count
    size_t getActiveDropCount() const { return activeDrops.size(); }
};

#endif // LOOT_SYSTEM_H
