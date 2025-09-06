#include "dungeon.h"
#include "character.h"
#include "race.h"
#include "class.h"
#include "ability.h"
#include "statblock.h"
#include "mob.h"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <sstream>
#include <random>

// Helper function implementations
DungeonTier getTierForLevel(leveltype level) {
    if (level <= 10) return DungeonTier::TIER_1;
    if (level <= 20) return DungeonTier::TIER_2;
    if (level <= 30) return DungeonTier::TIER_3;
    if (level <= 40) return DungeonTier::TIER_4;
    if (level <= 50) return DungeonTier::TIER_5;
    if (level <= 60) return DungeonTier::TIER_6;
    if (level <= 70) return DungeonTier::TIER_7;
    if (level <= 80) return DungeonTier::TIER_8;
    if (level <= 90) return DungeonTier::TIER_9;
    return DungeonTier::TIER_10;
}

std::string getTierName(DungeonTier tier) {
    switch (tier) {
        case DungeonTier::TIER_1: return "Novice";
        case DungeonTier::TIER_2: return "Apprentice";
        case DungeonTier::TIER_3: return "Journeyman";
        case DungeonTier::TIER_4: return "Expert";
        case DungeonTier::TIER_5: return "Master";
        case DungeonTier::TIER_6: return "Grandmaster";
        case DungeonTier::TIER_7: return "Legendary";
        case DungeonTier::TIER_8: return "Mythic";
        case DungeonTier::TIER_9: return "Divine";
        case DungeonTier::TIER_10: return "Transcendent";
        default: return "Unknown";
    }
}

leveltype getMinLevelForTier(DungeonTier tier) {
    switch (tier) {
        case DungeonTier::TIER_1: return 1;
        case DungeonTier::TIER_2: return 10;
        case DungeonTier::TIER_3: return 20;
        case DungeonTier::TIER_4: return 30;
        case DungeonTier::TIER_5: return 40;
        case DungeonTier::TIER_6: return 50;
        case DungeonTier::TIER_7: return 60;
        case DungeonTier::TIER_8: return 70;
        case DungeonTier::TIER_9: return 80;
        case DungeonTier::TIER_10: return 90;
        default: return 1;
    }
}

leveltype getMaxLevelForTier(DungeonTier tier) {
    switch (tier) {
        case DungeonTier::TIER_1: return 10;
        case DungeonTier::TIER_2: return 20;
        case DungeonTier::TIER_3: return 30;
        case DungeonTier::TIER_4: return 40;
        case DungeonTier::TIER_5: return 50;
        case DungeonTier::TIER_6: return 60;
        case DungeonTier::TIER_7: return 70;
        case DungeonTier::TIER_8: return 80;
        case DungeonTier::TIER_9: return 90;
        case DungeonTier::TIER_10: return 100;
        default: return 10;
    }
}

float getRarityChanceForTier(DungeonTier tier, ItemRarity rarity) {
    // Base chances for each rarity
    float baseChances[5] = {0.6f, 0.25f, 0.1f, 0.04f, 0.01f}; // Common to Legendary
    
    // Tier multiplier increases chances for higher rarities
    float tierMultiplier = 1.0f + (static_cast<int>(tier) * 0.1f);
    
    // Calculate adjusted chance
    float adjustedChance = baseChances[static_cast<int>(rarity)] * tierMultiplier;
    
    // Cap at reasonable maximum
    return std::min(adjustedChance, 0.5f);
}

// Stream operator implementations
std::ostream& operator<<(std::ostream& os, const RoomType& roomType) {
    switch (roomType) {
        case RoomType::ENTRANCE: return os << "ENTRANCE";
        case RoomType::COMBAT: return os << "COMBAT";
        case RoomType::TREASURE: return os << "TREASURE";
        case RoomType::BOSS: return os << "BOSS";
        case RoomType::CORRIDOR: return os << "CORRIDOR";
        case RoomType::REST: return os << "REST";
        case RoomType::PUZZLE: return os << "PUZZLE";
        case RoomType::EXIT: return os << "EXIT";
        default: return os << "UNKNOWN";
    }
}

std::ostream& operator<<(std::ostream& os, const DungeonTier& tier) {
    return os << getTierName(tier);
}

// Dungeon class implementation
Dungeon::Dungeon(const std::string& dungeonName, DungeonTier dungeonTier, 
                 leveltype minLvl, leveltype maxLvl)
    : name(dungeonName), tier(dungeonTier), minLevel(minLvl), maxLevel(maxLvl),
      bossLevel(maxLvl), isGenerated(false), dungeonCompleted(false), bossDefeated(false) {
    
    // Initialize random generator
    std::random_device rd;
    randomGenerator.seed(rd());
    
    // Set boss level to max level of dungeon
    bossLevel = maxLevel;
    
    // Initialize default boss
    boss = BossInfo("Dungeon Guardian", "A powerful guardian of this dungeon", bossLevel, Race::createDragon());
}

Dungeon::~Dungeon() = default;

void Dungeon::generate() {
    if (isGenerated) return;
    
    std::cout << "Generating dungeon: " << name << " (Tier: " << getTierName(tier) << ")" << std::endl;
    
    // Clear existing content
    rooms.clear();
    corridors.clear();
    enemies.clear();
    loot.clear();
    
    // Generate dungeon layout
    generateLayout();
    
    // Spawn enemies and loot
    spawnEnemies();
    spawnLoot();
    placeBoss();
    
    isGenerated = true;
    std::cout << "Dungeon generation complete!" << std::endl;
}

void Dungeon::generateLayout() {
    generateRooms();
    generateCorridors();
    placeRooms();
    connectRooms();
}

void Dungeon::generateRooms() {
    // Create entrance room
    DungeonRoom entranceRoom(RoomType::ENTRANCE, Position(0, 0, 0), 12, 12);
    entranceRoom.isAccessible = true;
    rooms.push_back(entranceRoom);
    
    // Create boss room (always at the end)
    double bossRoomSize = 20.0;
    Position bossRoomPos(100, 0, 100);
    DungeonRoom bossRoom(RoomType::BOSS, bossRoomPos, bossRoomSize, bossRoomSize);
    rooms.push_back(bossRoom);
    
    // Generate other rooms
    std::uniform_real_distribution<double> sizeDist(params.roomSizeMin, params.roomSizeMax);
    std::uniform_real_distribution<double> posDist(-50, 50);
    
    for (int i = 0; i < params.roomCount - 2; ++i) {
        double width = sizeDist(randomGenerator);
        double height = sizeDist(randomGenerator);
        
        Position roomPos = findValidRoomPosition(width, height);
        RoomType roomType = (i % 5 == 0) ? RoomType::TREASURE : 
                           (i % 7 == 0) ? RoomType::REST : RoomType::COMBAT;
        
        DungeonRoom room(roomType, roomPos, width, height);
        rooms.push_back(room);
    }
    
    // Generate spawn points for each room
    for (auto& room : rooms) {
        if (room.type == RoomType::COMBAT || room.type == RoomType::BOSS) {
            room.enemySpawnPoints = generateSpawnPoints(room, 
                std::uniform_int_distribution<int>(1, params.maxEnemiesPerRoom)(randomGenerator));
        }
        
        if (room.type == RoomType::TREASURE || room.type == RoomType::BOSS) {
            room.lootSpawnPoints = generateSpawnPoints(room, 
                std::uniform_int_distribution<int>(1, params.maxLootPerRoom)(randomGenerator));
        }
    }
}

void Dungeon::generateCorridors() {
    // Simple corridor generation - connect rooms in sequence
    for (size_t i = 0; i < rooms.size() - 1; ++i) {
        Position start = rooms[i].position;
        Position end = rooms[i + 1].position;
        
        // Create corridor points
        double corridorLength = start.distanceTo(end);
        int corridorPoints = static_cast<int>(corridorLength / params.corridorWidth);
        
        for (int j = 0; j <= corridorPoints; ++j) {
            double t = static_cast<double>(j) / corridorPoints;
            Position corridorPoint = start + (end - start) * t;
            corridors.push_back(corridorPoint);
        }
    }
}

void Dungeon::placeRooms() {
    // Rooms are already positioned in generateRooms()
    // This method could be extended for more complex room placement logic
}

void Dungeon::connectRooms() {
    // Rooms are connected via corridors in generateCorridors()
    // This method could be extended for more complex connection logic
}

void Dungeon::spawnEnemies() {
    // Generate enemies based on dungeon tier and level
    std::uniform_int_distribution<int> enemyCountDist(3, 8);
    int totalEnemies = enemyCountDist(randomGenerator);
    
    for (int i = 0; i < totalEnemies; ++i) {
        // Choose random room for spawning
        std::uniform_int_distribution<size_t> roomDist(0, rooms.size() - 1);
        size_t roomIndex = roomDist(randomGenerator);
        
        if (rooms[roomIndex].type == RoomType::COMBAT && !rooms[roomIndex].enemySpawnPoints.empty()) {
            // Choose random spawn point
            std::uniform_int_distribution<size_t> spawnDist(0, rooms[roomIndex].enemySpawnPoints.size() - 1);
            size_t spawnIndex = spawnDist(randomGenerator);
            Position spawnPos = rooms[roomIndex].enemySpawnPoints[spawnIndex];
            
            // Create enemy based on dungeon tier
            Race enemyRace;
            switch (i % 4) {
                case 0: enemyRace = Race::createGoblin(); break;
                case 1: enemyRace = Race::createOrc(); break;
                case 2: enemyRace = Race::createUndead(); break;
                case 3: enemyRace = Race::createBeast(); break;
            }
            
            Mob enemy(enemyRace);
            enemy.setPosition(spawnPos.getX(), spawnPos.getY(), spawnPos.getZ());
            
            // Scale enemy stats based on dungeon level
            StatBlock& stats = enemy.getStatsRef();
            float levelMultiplier = 1.0f + (static_cast<float>(minLevel) / 10.0f);
            stats.setMaxHealth(static_cast<welltype>(stats.getMaxHealth() * levelMultiplier));
            stats.setMaxMana(static_cast<welltype>(stats.getMaxMana() * levelMultiplier));
            stats.setHealth(stats.getMaxHealth());
            stats.setMana(stats.getMaxMana());
            
            enemies.push_back(enemy);
        }
    }
}

void Dungeon::spawnLoot() {
    // Generate loot based on dungeon tier
    std::uniform_int_distribution<int> lootCountDist(5, 15);
    int totalLoot = lootCountDist(randomGenerator);
    
    for (int i = 0; i < totalLoot; ++i) {
        // Choose random room for spawning
        std::uniform_int_distribution<size_t> roomDist(0, rooms.size() - 1);
        size_t roomIndex = roomDist(randomGenerator);
        
        if (rooms[roomIndex].type == RoomType::TREASURE && !rooms[roomIndex].lootSpawnPoints.empty()) {
            // Choose random spawn point
            std::uniform_int_distribution<size_t> spawnDist(0, rooms[roomIndex].lootSpawnPoints.size() - 1);
            size_t spawnIndex = spawnDist(randomGenerator);
            Position spawnPos = rooms[roomIndex].lootSpawnPoints[spawnIndex];
            
            // Generate item based on dungeon tier
            ItemRarity rarity = ItemRarity::COMMON;
            float rarityRoll = std::uniform_real_distribution<float>(0.0f, 1.0f)(randomGenerator);
            
            if (rarityRoll < getRarityChanceForTier(tier, ItemRarity::LEGENDARY)) {
                rarity = ItemRarity::LEGENDARY;
            } else if (rarityRoll < getRarityChanceForTier(tier, ItemRarity::EPIC)) {
                rarity = ItemRarity::EPIC;
            } else if (rarityRoll < getRarityChanceForTier(tier, ItemRarity::RARE)) {
                rarity = ItemRarity::RARE;
            } else if (rarityRoll < getRarityChanceForTier(tier, ItemRarity::UNCOMMON)) {
                rarity = ItemRarity::UNCOMMON;
            }
            
            // Create random item
            std::uniform_int_distribution<int> itemTypeDist(0, 3);
            int itemType = itemTypeDist(randomGenerator);
            
            Item lootItem;
            switch (itemType) {
                case 0: // Weapon
                    lootItem = Item::createSword("Dungeon Weapon", rarity);
                    break;
                case 1: // Armor
                    lootItem = Item::createArmor("Dungeon Armor", ArmorType::CHESTPLATE, rarity);
                    break;
                case 2: // Potion
                    lootItem = Item::createPotion("Dungeon Potion", 100, 50);
                    break;
                case 3: // Material
                    lootItem = Item::createMaterial("Dungeon Material");
                    break;
            }
            
            // Set level requirements based on dungeon
            lootItem.setRequiredLevel(minLevel);
            
            loot.push_back(lootItem);
        }
    }
}

void Dungeon::placeBoss() {
    // Boss is already positioned in the boss room
    // This method could be extended for more complex boss placement logic
    
    // Set boss stats based on dungeon level
    StatBlock& bossStats = boss.stats;
    float bossMultiplier = 2.0f + (static_cast<float>(bossLevel) / 10.0f);
    bossStats.setMaxHealth(static_cast<welltype>(bossStats.getMaxHealth() * bossMultiplier));
    bossStats.setMaxMana(static_cast<welltype>(bossStats.getMaxMana() * bossMultiplier));
    bossStats.setHealth(bossStats.getMaxHealth());
    bossStats.setMana(bossStats.getMaxMana());
}

void Dungeon::reset() {
    isGenerated = false;
    dungeonCompleted = false;
    bossDefeated = false;
    
    // Clear all content
    rooms.clear();
    corridors.clear();
    enemies.clear();
    loot.clear();
    
    std::cout << "Dungeon " << name << " has been reset." << std::endl;
}

bool Dungeon::enter(Character& player) {
    if (!isGenerated) {
        generate();
    }
    
    if (!isAccessible(player.getStats().getLevel())) {
        std::cout << "You are not high enough level to enter this dungeon!" << std::endl;
        return false;
    }
    
    // Teleport player to entrance
    player.setPosition(entrance.getX(), entrance.getY(), entrance.getZ());
    
    std::cout << "You enter " << name << " (Tier: " << getTierName(tier) << ")" << std::endl;
    std::cout << "Level range: " << minLevel << "-" << maxLevel << std::endl;
    std::cout << "Boss level: " << bossLevel << std::endl;
    
    return true;
}

bool Dungeon::exit(Character& player) {
    // Teleport player back to world
    player.setPosition(0, 0, 0);
    
    std::cout << "You exit " << name << std::endl;
    return true;
}

bool Dungeon::isAccessible(leveltype playerLevel) const {
    return playerLevel >= minLevel && playerLevel <= maxLevel;
}

bool Dungeon::isPlayerInDungeon() const {
    // This would need to be tracked by the game engine
    // For now, return false
    return false;
}

void Dungeon::spawnEnemy(const Mob& enemy, const Position& position) {
    Mob newEnemy = enemy;
    newEnemy.setPosition(position.getX(), position.getY(), position.getZ());
    enemies.push_back(newEnemy);
}

void Dungeon::removeEnemy(const Mob& enemy) {
    enemies.erase(std::remove(enemies.begin(), enemies.end(), enemy), enemies.end());
}

std::vector<Mob> Dungeon::getEnemiesInRoom(const Position& roomCenter, double radius) {
    std::vector<Mob> roomEnemies;
    for (const auto& enemy : enemies) {
        if (enemy.getPosition().distanceTo(roomCenter) <= radius) {
            roomEnemies.push_back(enemy);
        }
    }
    return roomEnemies;
}

void Dungeon::addLoot(const Item& item, const Position& position) {
    loot.push_back(item);
}

void Dungeon::removeLoot(const Item& item) {
    loot.erase(std::remove(loot.begin(), loot.end(), item), loot.end());
}

std::vector<Item> Dungeon::getLootInRoom(const Position& roomCenter, double radius) {
    // For now, return all loot since we don't have room-specific positioning
    return loot;
}

void Dungeon::setBoss(const BossInfo& bossInfo) {
    boss = bossInfo;
}

void Dungeon::defeatBoss() {
    bossDefeated = true;
    dungeonCompleted = true;
    std::cout << "Boss defeated! Dungeon completed!" << std::endl;
}

std::string Dungeon::getDescription() const {
    std::ostringstream oss;
    oss << "=== " << name << " ===\n";
    oss << "Tier: " << getTierName(tier) << "\n";
    oss << "Level Range: " << minLevel << "-" << maxLevel << "\n";
    oss << "Boss Level: " << bossLevel << "\n";
    oss << "Rooms: " << rooms.size() << "\n";
    oss << "Enemies: " << enemies.size() << "\n";
    oss << "Loot: " << loot.size() << "\n";
    oss << "Status: " << (dungeonCompleted ? "Completed" : (bossDefeated ? "Boss Defeated" : "In Progress")) << "\n";
    return oss.str();
}

void Dungeon::printLayout() const {
    std::cout << "=== Dungeon Layout: " << name << " ===" << std::endl;
    for (size_t i = 0; i < rooms.size(); ++i) {
        const auto& room = rooms[i];
        std::cout << "Room " << i << ": " << room.type << " at " << room.position 
                  << " (" << room.width << "x" << room.height << ")" << std::endl;
        std::cout << "  - Enemies: " << room.enemySpawnPoints.size() << std::endl;
        std::cout << "  - Loot: " << room.lootSpawnPoints.size() << std::endl;
        std::cout << "  - Cleared: " << (room.isCleared ? "Yes" : "No") << std::endl;
    }
    std::cout << "Corridors: " << corridors.size() << " points" << std::endl;
}

Position Dungeon::getRandomPosition() const {
    if (rooms.empty()) return Position(0, 0, 0);
    
    // Use a local random generator for const methods
    static std::mt19937 localGenerator;
    static bool initialized = false;
    if (!initialized) {
        std::random_device rd;
        localGenerator.seed(rd());
        initialized = true;
    }
    
    std::uniform_int_distribution<size_t> roomDist(0, rooms.size() - 1);
    size_t roomIndex = roomDist(localGenerator);
    const auto& room = rooms[roomIndex];
    
    std::uniform_real_distribution<double> xDist(room.position.getX() - room.width/2, room.position.getX() + room.width/2);
    std::uniform_real_distribution<double> zDist(room.position.getZ() - room.height/2, room.position.getZ() + room.height/2);
    
    return Position(xDist(localGenerator), room.position.getY(), zDist(localGenerator));
}

// Utility methods
bool Dungeon::isPositionValid(const Position& pos, double width, double height) {
    // Check if position is within reasonable bounds
    if (std::abs(pos.getX()) > 200 || std::abs(pos.getZ()) > 200) return false;
    
    // Check overlap with existing rooms
    for (const auto& room : rooms) {
        if (roomsOverlap(DungeonRoom(RoomType::COMBAT, pos, width, height), room)) {
            return false;
        }
    }
    
    return true;
}

bool Dungeon::roomsOverlap(const DungeonRoom& room1, const DungeonRoom& room2) {
    double x1 = room1.position.getX() - room1.width/2;
    double x2 = room1.position.getX() + room1.width/2;
    double z1 = room1.position.getZ() - room1.height/2;
    double z2 = room1.position.getZ() + room1.height/2;
    
    double x3 = room2.position.getX() - room2.width/2;
    double x4 = room2.position.getX() + room2.width/2;
    double z3 = room2.position.getZ() - room2.height/2;
    double z4 = room2.position.getZ() + room2.height/2;
    
    return !(x2 < x3 || x4 < x1 || z2 < z3 || z4 < z1);
}

Position Dungeon::findValidRoomPosition(double width, double height) {
    std::uniform_real_distribution<double> posDist(-100, 100);
    
    for (int attempts = 0; attempts < 100; ++attempts) {
        Position pos(posDist(randomGenerator), 0, posDist(randomGenerator));
        if (isPositionValid(pos, width, height)) {
            return pos;
        }
    }
    
    // Fallback position
    return Position(0, 0, 0);
}

std::vector<Position> Dungeon::generateSpawnPoints(const DungeonRoom& room, int count) {
    std::vector<Position> spawnPoints;
    std::uniform_real_distribution<double> xDist(room.position.getX() - room.width/2 + 1, room.position.getX() + room.width/2 - 1);
    std::uniform_real_distribution<double> zDist(room.position.getZ() - room.height/2 + 1, room.position.getZ() + room.height/2 - 1);
    
    for (int i = 0; i < count; ++i) {
        Position spawnPos(xDist(randomGenerator), room.position.getY(), zDist(randomGenerator));
        spawnPoints.push_back(spawnPos);
    }
    
    return spawnPoints;
}

// DungeonManager implementation
DungeonManager::DungeonManager(std::shared_ptr<LootSystem> lootSys)
    : lootSystem(lootSys) {
    
    std::random_device rd;
    randomGenerator.seed(rd());
    
    generatePortals();
}

DungeonManager::~DungeonManager() = default;

void DungeonManager::generatePortals() {
    // Create portals for different dungeon tiers
    placePortal("Goblin Mines", DungeonTier::TIER_1, 1, 10, Position(50, 0, 50));
    placePortal("Orc Stronghold", DungeonTier::TIER_2, 10, 20, Position(100, 0, 100));
    placePortal("Undead Crypt", DungeonTier::TIER_3, 20, 30, Position(150, 0, 150));
    placePortal("Dragon's Lair", DungeonTier::TIER_4, 30, 40, Position(200, 0, 200));
    placePortal("Demon Gate", DungeonTier::TIER_5, 40, 50, Position(250, 0, 250));
}

void DungeonManager::placePortal(const std::string& dungeonName, DungeonTier tier, 
                                 leveltype minLevel, leveltype maxLevel, const Position& worldPos) {
    DungeonPortal portal(dungeonName, tier, minLevel, maxLevel, worldPos);
    portals.push_back(portal);
}

void DungeonManager::addDungeon(std::shared_ptr<Dungeon> dungeon) {
    dungeons[dungeon->getName()] = dungeon;
}

std::shared_ptr<Dungeon> DungeonManager::getDungeon(const std::string& name) {
    auto it = dungeons.find(name);
    return (it != dungeons.end()) ? it->second : nullptr;
}

std::vector<std::string> DungeonManager::getAvailableDungeons(leveltype playerLevel) {
    std::vector<std::string> available;
    
    for (const auto& portal : portals) {
        if (portal.isActive && playerLevel >= portal.minLevel && playerLevel <= portal.maxLevel) {
            available.push_back(portal.dungeonName);
        }
    }
    
    return available;
}

void DungeonManager::addPortal(const DungeonPortal& portal) {
    portals.push_back(portal);
}

std::vector<DungeonPortal> DungeonManager::getPortalsInRange(const Position& playerPos, double range) {
    std::vector<DungeonPortal> nearbyPortals;
    
    for (const auto& portal : portals) {
        if (portal.isActive && playerPos.distanceTo(portal.worldPosition) <= range) {
            nearbyPortals.push_back(portal);
        }
    }
    
    return nearbyPortals;
}

bool DungeonManager::enterDungeon(Character& player, const std::string& dungeonName) {
    auto dungeon = getDungeon(dungeonName);
    if (!dungeon) {
        // Create dungeon if it doesn't exist
        auto portalIt = std::find_if(portals.begin(), portals.end(),
            [&](const DungeonPortal& p) { return p.dungeonName == dungeonName; });
        
        if (portalIt != portals.end()) {
            dungeon = std::make_shared<Dungeon>(dungeonName, portalIt->tier, 
                                               portalIt->minLevel, portalIt->maxLevel);
            addDungeon(dungeon);
        } else {
            std::cout << "Dungeon not found: " << dungeonName << std::endl;
            return false;
        }
    }
    
    return dungeon->enter(player);
}

void DungeonManager::generateAllDungeons() {
    for (auto& dungeonPair : dungeons) {
        dungeonPair.second->generate();
    }
}

void DungeonManager::resetAllDungeons() {
    for (auto& dungeonPair : dungeons) {
        dungeonPair.second->reset();
    }
}

std::string DungeonManager::getDungeonInfo(const std::string& name) const {
    auto it = dungeons.find(name);
    if (it != dungeons.end()) {
        return it->second->getDescription();
    }
    
    // Check portals for dungeon info
    for (const auto& portal : portals) {
        if (portal.dungeonName == name) {
            std::ostringstream oss;
            oss << "=== " << portal.dungeonName << " ===\n";
            oss << "Tier: " << getTierName(portal.tier) << "\n";
            oss << "Level Range: " << portal.minLevel << "-" << portal.maxLevel << "\n";
            oss << "Location: " << portal.worldPosition << "\n";
            oss << "Description: " << portal.description << "\n";
            oss << "Status: " << (portal.isActive ? "Active" : "Inactive") << "\n";
            return oss.str();
        }
    }
    
    return "Dungeon not found: " + name;
}

void DungeonManager::printAllDungeons() const {
    std::cout << "=== Available Dungeons ===" << std::endl;
    for (const auto& portal : portals) {
        std::cout << portal.dungeonName << " (Tier: " << getTierName(portal.tier) 
                  << ", Levels " << portal.minLevel << "-" << portal.maxLevel << ")" << std::endl;
    }
    
    std::cout << "\n=== Generated Dungeons ===" << std::endl;
    for (const auto& dungeonPair : dungeons) {
        std::cout << dungeonPair.first << " - " 
                  << (dungeonPair.second->isCompleted() ? "Completed" : "In Progress") << std::endl;
    }
}
