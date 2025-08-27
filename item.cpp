#include "item.h"
#include <iostream>
#include <sstream>

// Default constructor
Item::Item() : 
    name("Unknown Item"),
    description("An unknown item"),
    type(ItemType::MISC),
    rarity(ItemRarity::COMMON),
    weaponType(WeaponType::NONE),
    armorType(ArmorType::NONE),
    strength(0),
    dexterity(0),
    intelligence(0),
    healthBonus(0),
    manaBonus(0),
    damage(0),
    armor(0),
    durability(100),
    maxDurability(100),
    quantity(1),
    maxStack(1),
    goldValue(0),
    requiredLevel(1),
    requiredStrength(0),
    requiredDexterity(0),
    requiredIntelligence(0) {
}

// Basic constructor
Item::Item(const std::string& name, const std::string& description, ItemType type) :
    name(name),
    description(description),
    type(type),
    rarity(ItemRarity::COMMON),
    weaponType(WeaponType::NONE),
    armorType(ArmorType::NONE),
    strength(0),
    dexterity(0),
    intelligence(0),
    healthBonus(0),
    manaBonus(0),
    damage(0),
    armor(0),
    durability(100),
    maxDurability(100),
    quantity(1),
    maxStack(1),
    goldValue(0),
    requiredLevel(1),
    requiredStrength(0),
    requiredDexterity(0),
    requiredIntelligence(0) {
}

// Full constructor
Item::Item(const std::string& name, const std::string& description, ItemType type, ItemRarity rarity) :
    name(name),
    description(description),
    type(type),
    rarity(rarity),
    weaponType(WeaponType::NONE),
    armorType(ArmorType::NONE),
    strength(0),
    dexterity(0),
    intelligence(0),
    healthBonus(0),
    manaBonus(0),
    damage(0),
    armor(0),
    durability(100),
    maxDurability(100),
    quantity(1),
    maxStack(1),
    goldValue(0),
    requiredLevel(1),
    requiredStrength(0),
    requiredDexterity(0),
    requiredIntelligence(0) {
}

// Utility methods
bool Item::isStackable() const {
    return maxStack > 1;
}

bool Item::canStackWith(const Item& other) const {
    if (!isStackable() || !other.isStackable()) return false;
    if (name != other.name) return false;
    if (type != other.type) return false;
    if (rarity != other.rarity) return false;
    return true;
}

bool Item::addToStack(stattype amount) {
    if (!isStackable()) return false;
    if (quantity + amount > maxStack) return false;
    quantity += amount;
    return true;
}

bool Item::removeFromStack(stattype amount) {
    if (amount > quantity) return false;
    quantity -= amount;
    return true;
}

bool Item::isBroken() const {
    return durability <= 0;
}

void Item::repair() {
    durability = maxDurability;
}

void Item::damageItem(stattype amount) {
    if (durability > amount) {
        durability -= amount;
    } else {
        durability = 0;
    }
}

// Factory methods
Item Item::createSword(const std::string& name, ItemRarity rarity) {
    Item sword(name, "A sharp blade for combat", ItemType::WEAPON, rarity);
    sword.setWeaponType(WeaponType::SWORD);
    sword.setDamage(10 + static_cast<stattype>(rarity) * 5);
    sword.setDurability(100);
    sword.setMaxDurability(100);
    sword.setMaxStack(1);
    sword.setGoldValue(50 + static_cast<stattype>(rarity) * 25);
    
    // Set requirements based on rarity
    sword.setRequiredLevel(1 + static_cast<leveltype>(rarity));
    sword.setRequiredStrength(5 + static_cast<stattype>(rarity) * 2);
    
    return sword;
}

Item Item::createArmor(const std::string& name, ArmorType armorType, ItemRarity rarity) {
    Item armor(name, "Protective gear", ItemType::ARMOR, rarity);
    armor.setArmorType(armorType);
    armor.setArmor(5 + static_cast<stattype>(rarity) * 3);
    armor.setDurability(100);
    armor.setMaxDurability(100);
    armor.setMaxStack(1);
    armor.setGoldValue(30 + static_cast<stattype>(rarity) * 20);
    
    // Set requirements based on rarity
    armor.setRequiredLevel(1 + static_cast<leveltype>(rarity));
    armor.setRequiredStrength(3 + static_cast<stattype>(rarity));
    
    return armor;
}

Item Item::createPotion(const std::string& name, welltype healthRestore, welltype manaRestore) {
    Item potion(name, "A magical potion", ItemType::CONSUMABLE, ItemRarity::COMMON);
    potion.setHealthBonus(healthRestore);
    potion.setManaBonus(manaRestore);
    potion.setMaxStack(10);
    potion.setGoldValue(25);
    potion.setRequiredLevel(1);
    
    return potion;
}

Item Item::createMaterial(const std::string& name, stattype maxStack) {
    Item material(name, "A crafting material", ItemType::MATERIAL, ItemRarity::COMMON);
    material.setMaxStack(maxStack);
    material.setGoldValue(5);
    material.setRequiredLevel(1);
    
    return material;
}

// Display methods
std::string Item::getRarityString() const {
    switch (rarity) {
        case ItemRarity::COMMON: return "Common";
        case ItemRarity::UNCOMMON: return "Uncommon";
        case ItemRarity::RARE: return "Rare";
        case ItemRarity::EPIC: return "Epic";
        case ItemRarity::LEGENDARY: return "Legendary";
        default: return "Unknown";
    }
}

std::string Item::getTypeString() const {
    switch (type) {
        case ItemType::WEAPON: return "Weapon";
        case ItemType::ARMOR: return "Armor";
        case ItemType::CONSUMABLE: return "Consumable";
        case ItemType::MATERIAL: return "Material";
        case ItemType::QUEST: return "Quest";
        case ItemType::MISC: return "Misc";
        default: return "Unknown";
    }
}

std::string Item::getFullDescription() const {
    std::stringstream ss;
    ss << name << " (" << getRarityString() << " " << getTypeString() << ")\n";
    ss << description << "\n";
    
    if (type == ItemType::WEAPON) {
        ss << "Damage: " << damage << "\n";
        ss << "Durability: " << durability << "/" << maxDurability << "\n";
    } else if (type == ItemType::ARMOR) {
        ss << "Armor: " << armor << "\n";
        ss << "Durability: " << durability << "/" << maxDurability << "\n";
    }
    
    if (strength > 0) ss << "Strength: +" << strength << "\n";
    if (dexterity > 0) ss << "Dexterity: +" << dexterity << "\n";
    if (intelligence > 0) ss << "Intelligence: +" << intelligence << "\n";
    if (healthBonus > 0) ss << "Health: +" << healthBonus << "\n";
    if (manaBonus > 0) ss << "Mana: +" << manaBonus << "\n";
    
    if (isStackable()) {
        ss << "Quantity: " << quantity << "/" << maxStack << "\n";
    }
    
    ss << "Value: " << goldValue << " gold\n";
    ss << "Required Level: " << requiredLevel << "\n";
    
    if (requiredStrength > 0) ss << "Required Strength: " << requiredStrength << "\n";
    if (requiredDexterity > 0) ss << "Required Dexterity: " << requiredDexterity << "\n";
    if (requiredIntelligence > 0) ss << "Required Intelligence: " << requiredIntelligence << "\n";
    
    return ss.str();
}

// Comparison operators
bool Item::operator==(const Item& other) const {
    return name == other.name && 
           type == other.type && 
           rarity == other.rarity &&
           weaponType == other.weaponType &&
           armorType == other.armorType;
}

bool Item::operator!=(const Item& other) const {
    return !(*this == other);
}



