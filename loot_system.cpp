#include "loot_system.h"
#include "mob.h"
#include <algorithm>
#include <cmath>

// LootTable Implementation
LootTable::LootTable(const std::string& name, float guaranteedChance, stattype maxItems)
    : tableName(name), guaranteedDropChance(guaranteedChance), maxDrops(maxItems) {}

void LootTable::addEntry(const LootEntry& entry) {
    entries.push_back(entry);
}

void LootTable::addEntry(const Item& item, float chance, stattype minQty, stattype maxQty,
                         leveltype minLvl, leveltype maxLvl) {
    entries.emplace_back(item, chance, minQty, maxQty, minLvl, maxLvl);
}

std::vector<Item> LootTable::generateLoot(leveltype mobLevel, std::mt19937& rng) const {
    std::vector<Item> droppedItems;
    std::uniform_real_distribution<float> chanceDist(0.0f, 1.0f);
    
    // Check if we should drop anything at all
    if (chanceDist(rng) > guaranteedDropChance) {
        return droppedItems;
    }
    
    // Determine how many items to drop
    std::uniform_int_distribution<stattype> dropCountDist(1, maxDrops);
    stattype itemsToDrop = dropCountDist(rng);
    
    // Try to drop items based on their individual chances
    for (const auto& entry : entries) {
        if (droppedItems.size() >= itemsToDrop) break;
        
        // Check level requirements
        if (mobLevel < entry.minLevel || mobLevel > entry.maxLevel) continue;
        
        // Check drop chance
        if (chanceDist(rng) <= entry.dropChance) {
            // Determine quantity
            std::uniform_int_distribution<stattype> qtyDist(entry.minQuantity, entry.maxQuantity);
            stattype quantity = qtyDist(rng);
            
            // Create item copy with quantity
            Item droppedItem = entry.item;
            droppedItem.setQuantity(quantity);
            droppedItems.push_back(droppedItem);
        }
    }
    
    return droppedItems;
}

// ClassEquipmentGenerator Implementation
ClassEquipmentGenerator::ClassEquipmentGenerator() {
    initializeWarriorEquipment();
    initializeMageEquipment();
    initializeArcherEquipment();
    initializePaladinEquipment();
}

void ClassEquipmentGenerator::initializeWarriorEquipment() {
    // Warrior weapons
    warriorEquipment["weapons"] = {
        Item::createSword("Iron Sword", ItemRarity::COMMON),
        Item::createSword("Steel Sword", ItemRarity::UNCOMMON),
        Item::createSword("Mithril Sword", ItemRarity::RARE),
        Item::createSword("Dragon Slayer", ItemRarity::EPIC),
        Item::createSword("Excalibur", ItemRarity::LEGENDARY)
    };
    
    // Warrior armor
    warriorEquipment["armor"] = {
        Item::createArmor("Iron Helmet", ArmorType::HELMET, ItemRarity::COMMON),
        Item::createArmor("Steel Helmet", ArmorType::HELMET, ItemRarity::UNCOMMON),
        Item::createArmor("Mithril Helmet", ArmorType::HELMET, ItemRarity::RARE),
        Item::createArmor("Dragon Helm", ArmorType::HELMET, ItemRarity::EPIC),
        Item::createArmor("Crown of Valor", ArmorType::HELMET, ItemRarity::LEGENDARY)
    };
    
    // Set warrior-specific stats for weapons
    for (auto& weapon : warriorEquipment["weapons"]) {
        weapon.setRequiredStrength(10);
        weapon.setRequiredDexterity(5);
        weapon.setRequiredIntelligence(0);
    }
    
    // Set warrior-specific stats for armor
    for (auto& armor : warriorEquipment["armor"]) {
        armor.setRequiredStrength(8);
        armor.setRequiredDexterity(3);
        armor.setRequiredIntelligence(0);
    }
}

void ClassEquipmentGenerator::initializeMageEquipment() {
    // Mage weapons
    mageEquipment["weapons"] = {
        Item::createStaff("Apprentice Staff", ItemRarity::COMMON),
        Item::createStaff("Mage Staff", ItemRarity::UNCOMMON),
        Item::createStaff("Archmage Staff", ItemRarity::RARE),
        Item::createStaff("Staff of the Void", ItemRarity::EPIC),
        Item::createStaff("Staff of Ultimate Power", ItemRarity::LEGENDARY)
    };
    
    // Mage armor
    mageEquipment["armor"] = {
        Item::createArmor("Cloth Robe", ArmorType::CHESTPLATE, ItemRarity::COMMON),
        Item::createArmor("Silk Robe", ArmorType::CHESTPLATE, ItemRarity::UNCOMMON),
        Item::createArmor("Mystic Robe", ArmorType::CHESTPLATE, ItemRarity::RARE),
        Item::createArmor("Robe of the Archmage", ArmorType::CHESTPLATE, ItemRarity::EPIC),
        Item::createArmor("Robe of the Void", ArmorType::CHESTPLATE, ItemRarity::LEGENDARY)
    };
    
    // Set mage-specific stats for weapons
    for (auto& weapon : mageEquipment["weapons"]) {
        weapon.setRequiredStrength(0);
        weapon.setRequiredDexterity(3);
        weapon.setRequiredIntelligence(15);
        weapon.setWeaponType(WeaponType::STAFF);
    }
    
    // Set mage-specific stats for armor
    for (auto& armor : mageEquipment["armor"]) {
        armor.setRequiredStrength(0);
        armor.setRequiredDexterity(2);
        armor.setRequiredIntelligence(12);
    }
}

void ClassEquipmentGenerator::initializeArcherEquipment() {
    // Archer weapons
    archerEquipment["weapons"] = {
        Item::createBow("Short Bow", ItemRarity::COMMON),
        Item::createBow("Long Bow", ItemRarity::UNCOMMON),
        Item::createBow("Composite Bow", ItemRarity::RARE),
        Item::createBow("Eagle Eye Bow", ItemRarity::EPIC),
        Item::createBow("Bow of the Wind", ItemRarity::LEGENDARY)
    };
    
    // Archer armor
    archerEquipment["armor"] = {
        Item::createArmor("Leather Armor", ArmorType::CHESTPLATE, ItemRarity::COMMON),
        Item::createArmor("Studded Leather", ArmorType::CHESTPLATE, ItemRarity::UNCOMMON),
        Item::createArmor("Shadow Armor", ArmorType::CHESTPLATE, ItemRarity::RARE),
        Item::createArmor("Night Stalker Armor", ArmorType::CHESTPLATE, ItemRarity::EPIC),
        Item::createArmor("Armor of the Wind", ArmorType::CHESTPLATE, ItemRarity::LEGENDARY)
    };
    
    // Set archer-specific stats for weapons
    for (auto& weapon : archerEquipment["weapons"]) {
        weapon.setRequiredStrength(3);
        weapon.setRequiredDexterity(15);
        weapon.setRequiredIntelligence(5);
        weapon.setWeaponType(WeaponType::BOW);
    }
    
    // Set archer-specific stats for armor
    for (auto& armor : archerEquipment["armor"]) {
        armor.setRequiredStrength(2);
        armor.setRequiredDexterity(10);
        armor.setRequiredIntelligence(3);
    }
}

void ClassEquipmentGenerator::initializePaladinEquipment() {
    // Paladin weapons
    paladinEquipment["weapons"] = {
        Item::createSword("Holy Sword", ItemRarity::COMMON),
        Item::createSword("Divine Blade", ItemRarity::UNCOMMON),
        Item::createSword("Sacred Sword", ItemRarity::RARE),
        Item::createSword("Blade of Light", ItemRarity::EPIC),
        Item::createSword("Excalibur of the Light", ItemRarity::LEGENDARY)
    };
    
    // Paladin armor
    paladinEquipment["armor"] = {
        Item::createArmor("Holy Armor", ArmorType::CHESTPLATE, ItemRarity::COMMON),
        Item::createArmor("Divine Armor", ArmorType::CHESTPLATE, ItemRarity::UNCOMMON),
        Item::createArmor("Sacred Armor", ArmorType::CHESTPLATE, ItemRarity::RARE),
        Item::createArmor("Armor of Light", ArmorType::CHESTPLATE, ItemRarity::EPIC),
        Item::createArmor("Armor of the Divine", ArmorType::CHESTPLATE, ItemRarity::LEGENDARY)
    };
    
    // Set paladin-specific stats for weapons
    for (auto& weapon : paladinEquipment["weapons"]) {
        weapon.setRequiredStrength(12);
        weapon.setRequiredDexterity(8);
        weapon.setRequiredIntelligence(8);
    }
    
    // Set paladin-specific stats for armor
    for (auto& armor : paladinEquipment["armor"]) {
        armor.setRequiredStrength(10);
        armor.setRequiredDexterity(5);
        armor.setRequiredIntelligence(8);
    }
}

Item ClassEquipmentGenerator::generateClassWeapon(const std::string& className, ItemRarity rarity, leveltype level) {
    std::map<std::string, std::vector<Item>>* equipmentMap = nullptr;
    
    if (className == "Warrior") equipmentMap = &warriorEquipment;
    else if (className == "Mage") equipmentMap = &mageEquipment;
    else if (className == "Archer") equipmentMap = &archerEquipment;
    else if (className == "Paladin") equipmentMap = &paladinEquipment;
    
    if (!equipmentMap || equipmentMap->find("weapons") == equipmentMap->end()) {
        return Item(); // Return empty item if class not found
    }
    
    // Find weapon of appropriate rarity
    for (const auto& weapon : (*equipmentMap)["weapons"]) {
        if (weapon.getRarity() == rarity) {
            Item generatedWeapon = weapon;
            
            // Scale stats based on level
            stattype levelMultiplier = std::max(1, static_cast<int>(level / 10));
            stattype baseDamage = weapon.getDamage() * levelMultiplier;
            
            // Apply rarity multiplier to stats
            float rarityMultiplier = getRarityMultiplier(rarity);
            generatedWeapon.setDamage(static_cast<stattype>(baseDamage * rarityMultiplier));
            generatedWeapon.setRequiredLevel(level);
            
            return generatedWeapon;
        }
    }
    
    return Item(); // Return empty item if rarity not found
}

Item ClassEquipmentGenerator::generateClassArmor(const std::string& className, ArmorType armorType, ItemRarity rarity, leveltype level) {
    std::map<std::string, std::vector<Item>>* equipmentMap = nullptr;
    
    if (className == "Warrior") equipmentMap = &warriorEquipment;
    else if (className == "Mage") equipmentMap = &mageEquipment;
    else if (className == "Archer") equipmentMap = &archerEquipment;
    else if (className == "Paladin") equipmentMap = &paladinEquipment;
    
    if (!equipmentMap || equipmentMap->find("armor") == equipmentMap->end()) {
        return Item(); // Return empty item if class not found
    }
    
    // Find armor of appropriate type and rarity
    for (const auto& armor : (*equipmentMap)["armor"]) {
        if (armor.getArmorType() == armorType && armor.getRarity() == rarity) {
            Item generatedArmor = armor;
            
            // Scale stats based on level
            stattype levelMultiplier = std::max(1, static_cast<int>(level / 10));
            stattype baseArmor = armor.getArmor() * levelMultiplier;
            
            // Apply rarity multiplier to stats
            float rarityMultiplier = getRarityMultiplier(rarity);
            generatedArmor.setArmor(static_cast<stattype>(baseArmor * rarityMultiplier));
            generatedArmor.setRequiredLevel(level);
            
            return generatedArmor;
        }
    }
    
    return Item(); // Return empty item if type/rarity not found
}

std::vector<Item> ClassEquipmentGenerator::getClassWeapons(const std::string& className, leveltype level) {
    std::vector<Item> availableWeapons;
    
    if (className == "Warrior") {
        for (const auto& weapon : warriorEquipment["weapons"]) {
            if (weapon.getRequiredLevel() <= level) {
                availableWeapons.push_back(weapon);
            }
        }
    } else if (className == "Mage") {
        for (const auto& weapon : mageEquipment["weapons"]) {
            if (weapon.getRequiredLevel() <= level) {
                availableWeapons.push_back(weapon);
            }
        }
    } else if (className == "Archer") {
        for (const auto& weapon : archerEquipment["weapons"]) {
            if (weapon.getRequiredLevel() <= level) {
                availableWeapons.push_back(weapon);
            }
        }
    } else if (className == "Paladin") {
        for (const auto& weapon : paladinEquipment["weapons"]) {
            if (weapon.getRequiredLevel() <= level) {
                availableWeapons.push_back(weapon);
            }
        }
    }
    
    return availableWeapons;
}

std::vector<Item> ClassEquipmentGenerator::getClassArmor(const std::string& className, leveltype level) {
    std::vector<Item> availableArmor;
    
    if (className == "Warrior") {
        for (const auto& armor : warriorEquipment["armor"]) {
            if (armor.getRequiredLevel() <= level) {
                availableArmor.push_back(armor);
            }
        }
    } else if (className == "Mage") {
        for (const auto& armor : mageEquipment["armor"]) {
            if (armor.getRequiredLevel() <= level) {
                availableArmor.push_back(armor);
            }
        }
    } else if (className == "Archer") {
        for (const auto& armor : archerEquipment["armor"]) {
            if (armor.getRequiredLevel() <= level) {
                availableArmor.push_back(armor);
            }
        }
    } else if (className == "Paladin") {
        for (const auto& armor : paladinEquipment["armor"]) {
            if (armor.getRequiredLevel() <= level) {
                availableArmor.push_back(armor);
            }
        }
    }
    
    return availableArmor;
}

bool ClassEquipmentGenerator::isItemSuitableForClass(const Item& item, const std::string& className) {
    if (className == "Warrior") {
        return item.getRequiredStrength() >= 8 && item.getRequiredIntelligence() <= 5;
    } else if (className == "Mage") {
        return item.getRequiredIntelligence() >= 12 && item.getRequiredStrength() <= 3;
    } else if (className == "Archer") {
        return item.getRequiredDexterity() >= 10 && item.getRequiredStrength() <= 5;
    } else if (className == "Paladin") {
        return item.getRequiredStrength() >= 8 && item.getRequiredIntelligence() >= 6;
    }
    
    return false;
}

float ClassEquipmentGenerator::getRarityMultiplier(ItemRarity rarity) {
    switch (rarity) {
        case ItemRarity::COMMON:
            return 1.0f;
        case ItemRarity::UNCOMMON:
            return 1.25f;
        case ItemRarity::RARE:
            return 1.5f;
        case ItemRarity::EPIC:
            return 2.0f;
        case ItemRarity::LEGENDARY:
            return 3.0f;
        default:
            return 1.0f;
    }
}

// LootSystem Implementation
LootSystem::LootSystem() : randomGenerator(std::random_device{}()) {
    classEquipmentGen = std::make_unique<ClassEquipmentGenerator>();
    initializeDefaultLootTables();
}

void LootSystem::initializeDefaultLootTables() {
    // Common loot table for basic mobs
    auto commonLoot = std::make_shared<LootTable>("Common", 0.6f, 2);
    commonLoot->addEntry(Item::createMaterial("Iron Ore", 99), 0.8f, 1, 3);
    commonLoot->addEntry(Item::createMaterial("Copper Ore", 99), 0.6f, 1, 2);
    commonLoot->addEntry(Item::createPotion("Health Potion", 25, 0), 0.4f, 1, 2);
    commonLoot->addEntry(Item::createPotion("Mana Potion", 0, 20), 0.3f, 1, 1);
    
    // Rare loot table for elite mobs
    auto rareLoot = std::make_shared<LootTable>("Rare", 0.9f, 4);
    rareLoot->addEntry(Item::createMaterial("Mithril Ore", 99), 0.7f, 1, 2);
    rareLoot->addEntry(Item::createMaterial("Gold Ore", 99), 0.5f, 1, 1);
    rareLoot->addEntry(Item::createPotion("Greater Health Potion", 50, 0), 0.6f, 1, 3);
    rareLoot->addEntry(Item::createPotion("Greater Mana Potion", 0, 40), 0.5f, 1, 2);
    
    // Boss loot table for boss mobs
    auto bossLoot = std::make_shared<LootTable>("Boss", 1.0f, 6);
    bossLoot->addEntry(Item::createMaterial("Dragon Scale", 99), 0.9f, 1, 3);
    bossLoot->addEntry(Item::createMaterial("Ancient Relic", 99), 0.8f, 1, 1);
    bossLoot->addEntry(Item::createPotion("Superior Health Potion", 100, 0), 1.0f, 2, 5);
    bossLoot->addEntry(Item::createPotion("Superior Mana Potion", 0, 80), 1.0f, 2, 4);
    
    addLootTable("Common", commonLoot);
    addLootTable("Rare", rareLoot);
    addLootTable("Boss", bossLoot);
}

void LootSystem::addLootTable(const std::string& name, std::shared_ptr<LootTable> table) {
    lootTables[name] = table;
}

std::shared_ptr<LootTable> LootSystem::getLootTable(const std::string& name) {
    auto it = lootTables.find(name);
    return (it != lootTables.end()) ? it->second : nullptr;
}

std::vector<Item> LootSystem::generateMobLoot(const Mob& mob) {
    // This would need Mob class to have getMobType() and getLevel() methods
    // For now, return basic loot
    return generateMobLoot("Common", 1);
}

std::vector<Item> LootSystem::generateMobLoot(const std::string& mobType, leveltype level) {
    std::vector<Item> loot;
    
    // Determine which loot table to use based on mob type
    std::string tableName = "Common";
    if (mobType.find("Elite") != std::string::npos || mobType.find("Rare") != std::string::npos) {
        tableName = "Rare";
    } else if (mobType.find("Boss") != std::string::npos) {
        tableName = "Boss";
    }
    
    auto table = getLootTable(tableName);
    if (table) {
        loot = table->generateLoot(level, randomGenerator);
    }
    
    return loot;
}

std::vector<Item> LootSystem::generateDungeonLoot(const std::string& dungeonTier, leveltype playerLevel) {
    std::vector<Item> loot;
    
    // Generate class-specific equipment based on dungeon tier
    ItemRarity rarity = generateRarity(playerLevel);
    
    // Add some guaranteed equipment drops for dungeons
    if (dungeonTier == "Tier1") {
        // Basic dungeon - common/uncommon loot
        rarity = (rarity == ItemRarity::COMMON) ? ItemRarity::UNCOMMON : rarity;
    } else if (dungeonTier == "Tier2") {
        // Medium dungeon - uncommon/rare loot
        rarity = (rarity == ItemRarity::COMMON) ? ItemRarity::RARE : 
                (rarity == ItemRarity::UNCOMMON) ? ItemRarity::RARE : rarity;
    } else if (dungeonTier == "Tier3") {
        // Hard dungeon - rare/epic loot
        rarity = (rarity == ItemRarity::COMMON) ? ItemRarity::EPIC :
                (rarity == ItemRarity::UNCOMMON) ? ItemRarity::EPIC :
                (rarity == ItemRarity::RARE) ? ItemRarity::EPIC : rarity;
    }
    
    // Add some materials and consumables
    loot.push_back(Item::createMaterial("Dungeon Key", 1));
    loot.push_back(Item::createPotion("Dungeon Health Potion", 75, 0));
    
    return loot;
}

std::vector<Item> LootSystem::generateClassSpecificLoot(const std::string& className, leveltype level, ItemRarity rarity) {
    std::vector<Item> loot;
    
    // Generate a weapon
    Item weapon = classEquipmentGen->generateClassWeapon(className, rarity, level);
    if (weapon.getName() != "") {
        loot.push_back(weapon);
    }
    
    // Generate armor pieces
    std::vector<ArmorType> armorTypes = {ArmorType::HELMET, ArmorType::CHESTPLATE, ArmorType::GAUNTLETS, ArmorType::GREAVES, ArmorType::BOOTS};
    
    for (const auto& armorType : armorTypes) {
        if (static_cast<float>(rand()) / RAND_MAX < 0.3f) { // 30% chance for each armor piece
            Item armor = classEquipmentGen->generateClassArmor(className, armorType, rarity, level);
            if (armor.getName() != "") {
                loot.push_back(armor);
            }
        }
    }
    
    return loot;
}

void LootSystem::setRandomSeed(unsigned int seed) {
    randomGenerator.seed(seed);
}

ItemRarity LootSystem::generateRarity(leveltype level, float luckBonus) {
    float rarityRoll = static_cast<float>(rand()) / RAND_MAX + luckBonus;
    
    // Level-based rarity scaling
    float levelBonus = static_cast<float>(level) / 100.0f;
    rarityRoll += levelBonus;
    
    if (rarityRoll > 0.95f) return ItemRarity::LEGENDARY;
    if (rarityRoll > 0.85f) return ItemRarity::EPIC;
    if (rarityRoll > 0.70f) return ItemRarity::RARE;
    if (rarityRoll > 0.50f) return ItemRarity::UNCOMMON;
    return ItemRarity::COMMON;
}

std::vector<std::string> LootSystem::getAvailableLootTables() const {
    std::vector<std::string> tableNames;
    for (const auto& pair : lootTables) {
        tableNames.push_back(pair.first);
    }
    return tableNames;
}

// LootDropManager Implementation
LootDropManager::LootDropManager(float cleanupInterval) 
    : cleanupInterval(cleanupInterval), lastCleanupTime(0.0f) {}

void LootDropManager::addLootDrop(const std::vector<Item>& items, const Position& position, float ttl, bool permanent) {
    activeDrops.emplace_back(items, position, ttl, permanent);
}

void LootDropManager::update(float deltaTime) {
    lastCleanupTime += deltaTime;
    
    if (lastCleanupTime >= cleanupInterval) {
        // Remove expired drops
        activeDrops.erase(
            std::remove_if(activeDrops.begin(), activeDrops.end(),
                [](const LootDropEvent& drop) {
                    return !drop.isPermanent && drop.timeToLive <= 0;
                }),
            activeDrops.end()
        );
        
        lastCleanupTime = 0.0f;
    }
    
    // Update remaining drops
    for (auto& drop : activeDrops) {
        if (!drop.isPermanent) {
            drop.timeToLive -= deltaTime;
        }
    }
}

std::vector<LootDropEvent> LootDropManager::getDropsInRadius(const Position& center, float radius) {
    std::vector<LootDropEvent> nearbyDrops;
    
    for (const auto& drop : activeDrops) {
        if (center.distanceTo(drop.dropPosition) <= radius) {
            nearbyDrops.push_back(drop);
        }
    }
    
    return nearbyDrops;
}

void LootDropManager::removeLootDrop(size_t index) {
    if (index < activeDrops.size()) {
        activeDrops.erase(activeDrops.begin() + index);
    }
}
