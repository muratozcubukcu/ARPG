#ifndef DUNGEON_H
#define DUNGEON_H

#include "position.h"
#include "race.h"
#include "statblock.h"
// Forward declaration
class Mob;
#include "item.h"
#include "loot_system.h"
#include "types.h"
#include <vector>
#include <memory>
#include <random>
#include <string>
#include <map>

// Forward declarations
class Character;
class LootSystem;
class Ability;

// Dungeon difficulty tiers
enum class DungeonTier {
    TIER_1,     // Levels 1-10
    TIER_2,     // Levels 10-20
    TIER_3,     // Levels 20-30
    TIER_4,     // Levels 30-40
    TIER_5,     // Levels 40-50
    TIER_6,     // Levels 50-60
    TIER_7,     // Levels 60-70
    TIER_8,     // Levels 70-80
    TIER_9,     // Levels 80-90
    TIER_10     // Levels 90-100
};

// Room types for procedural generation
enum class RoomType {
    ENTRANCE,       // Starting room
    COMBAT,         // Regular combat room
    TREASURE,       // Loot room
    BOSS,          // Boss room
    CORRIDOR,      // Connecting hallway
    REST,          // Safe room (healing)
    PUZZLE,        // Puzzle room
    EXIT           // Exit room
};

// Dungeon room structure
struct DungeonRoom {
    RoomType type;
    Position position;
    double width;
    double height;
    std::vector<Position> enemySpawnPoints;
    std::vector<Position> lootSpawnPoints;
    std::vector<Position> exitPoints;
    bool isCleared;
    bool isAccessible;
    
    DungeonRoom(RoomType t, const Position& pos, double w, double h)
        : type(t), position(pos), width(w), height(h), isCleared(false), isAccessible(false) {}
};

// Portal structure for dungeon access
struct DungeonPortal {
    std::string dungeonName;
    DungeonTier tier;
    leveltype minLevel;
    leveltype maxLevel;
    Position worldPosition;
    bool isActive;
    std::string description;
    
    DungeonPortal(const std::string& name, DungeonTier t, leveltype min, leveltype max, 
                  const Position& pos, const std::string& desc = "")
        : dungeonName(name), tier(t), minLevel(min), maxLevel(max), 
          worldPosition(pos), isActive(true), description(desc) {}
};

// Boss information
struct BossInfo {
    std::string name;
    std::string description;
    leveltype level;
    Race race;
    StatBlock stats;
    std::vector<Ability> abilities;
    std::vector<Item> guaranteedDrops;
    float legendaryDropChance;
    
    BossInfo(const std::string& n, const std::string& desc, leveltype lvl, Race r)
        : name(n), description(desc), level(lvl), race(r), legendaryDropChance(0.1f) {}
    
    BossInfo() : name(""), description(""), level(1), race(Race::createHuman()), legendaryDropChance(0.1f) {}
};

// Dungeon generation parameters
struct DungeonParams {
    int roomCount;
    double roomSizeMin;
    double roomSizeMax;
    double corridorWidth;
    int maxEnemiesPerRoom;
    int maxLootPerRoom;
    float bossRoomChance;
    float treasureRoomChance;
    
    DungeonParams() : roomCount(15), roomSizeMin(8.0), roomSizeMax(20.0), 
                      corridorWidth(3.0), maxEnemiesPerRoom(5), maxLootPerRoom(3),
                      bossRoomChance(0.1f), treasureRoomChance(0.2f) {}
};

// Main dungeon class
class Dungeon {
private:
    std::string name;
    DungeonTier tier;
    leveltype minLevel;
    leveltype maxLevel;
    leveltype bossLevel;
    
    // Dungeon layout
    std::vector<DungeonRoom> rooms;
    std::vector<Position> corridors;
    Position entrance;
    Position exitPosition;
    
    // Dungeon content
    std::vector<Mob> enemies;
    std::vector<Item> loot;
    BossInfo boss;
    
    // Generation parameters
    DungeonParams params;
    std::mt19937 randomGenerator;
    
    // State
    bool isGenerated;
    bool dungeonCompleted;
    bool bossDefeated;
    
    // Generation methods
    void generateLayout();
    void generateRooms();
    void generateCorridors();
    void placeRooms();
    void connectRooms();
    void spawnEnemies();
    void spawnLoot();
    void placeBoss();
    
    // Utility methods
    bool isPositionValid(const Position& pos, double width, double height);
    bool roomsOverlap(const DungeonRoom& room1, const DungeonRoom& room2);
    Position findValidRoomPosition(double width, double height);
    std::vector<Position> generateSpawnPoints(const DungeonRoom& room, int count);
    
public:
    Dungeon(const std::string& dungeonName, DungeonTier dungeonTier, 
            leveltype minLvl, leveltype maxLvl);
    ~Dungeon();
    
    // Core methods
    void generate();
    void reset();
    bool enter(Character& player);
    bool exit(Character& player);
    
    // Dungeon state
    bool isAccessible(leveltype playerLevel) const;
    bool isPlayerInDungeon() const;
    bool isBossDefeated() const { return bossDefeated; }
    bool isCompleted() const { return dungeonCompleted; }
    
    // Getters
    std::string getName() const { return name; }
    DungeonTier getTier() const { return tier; }
    leveltype getMinLevel() const { return minLevel; }
    leveltype getMaxLevel() const { return maxLevel; }
    leveltype getBossLevel() const { return bossLevel; }
    const std::vector<DungeonRoom>& getRooms() const { return rooms; }
    const std::vector<Mob>& getEnemies() const { return enemies; }
    const std::vector<Item>& getLoot() const { return loot; }
    const BossInfo& getBoss() const { return boss; }
    
    // Enemy management
    void spawnEnemy(const Mob& enemy, const Position& position);
    void removeEnemy(const Mob& enemy);
    std::vector<Mob> getEnemiesInRoom(const Position& roomCenter, double radius);
    
    // Loot management
    void addLoot(const Item& item, const Position& position);
    void removeLoot(const Item& item);
    std::vector<Item> getLootInRoom(const Position& roomCenter, double radius);
    
    // Boss management
    void setBoss(const BossInfo& bossInfo);
    void defeatBoss();
    
    // Utility
    std::string getDescription() const;
    void printLayout() const;
    Position getRandomPosition() const;
};

// Dungeon manager for handling multiple dungeons
class DungeonManager {
private:
    std::map<std::string, std::shared_ptr<Dungeon>> dungeons;
    std::vector<DungeonPortal> portals;
    std::shared_ptr<LootSystem> lootSystem;
    std::mt19937 randomGenerator;
    
    // Portal generation
    void generatePortals();
    void placePortal(const std::string& dungeonName, DungeonTier tier, 
                     leveltype minLevel, leveltype maxLevel, const Position& worldPos);
    
public:
    DungeonManager(std::shared_ptr<LootSystem> lootSys);
    ~DungeonManager();
    
    // Dungeon management
    void addDungeon(std::shared_ptr<Dungeon> dungeon);
    std::shared_ptr<Dungeon> getDungeon(const std::string& name);
    std::vector<std::string> getAvailableDungeons(leveltype playerLevel);
    
    // Portal management
    void addPortal(const DungeonPortal& portal);
    std::vector<DungeonPortal> getPortalsInRange(const Position& playerPos, double range);
    bool enterDungeon(Character& player, const std::string& dungeonName);
    
    // Dungeon generation
    void generateAllDungeons();
    void resetAllDungeons();
    
    // Utility
    std::string getDungeonInfo(const std::string& name) const;
    void printAllDungeons() const;
};

// Helper functions
DungeonTier getTierForLevel(leveltype level);
std::string getTierName(DungeonTier tier);
leveltype getMinLevelForTier(DungeonTier tier);
leveltype getMaxLevelForTier(DungeonTier tier);
float getRarityChanceForTier(DungeonTier tier, ItemRarity rarity);

// Stream operators for enums
std::ostream& operator<<(std::ostream& os, const RoomType& roomType);
std::ostream& operator<<(std::ostream& os, const DungeonTier& tier);

#endif // DUNGEON_H
