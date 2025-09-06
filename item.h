#ifndef ITEM_H
#define ITEM_H

#include "types.h"
#include <string>
#include <vector>

enum class ItemType {
    WEAPON,
    ARMOR,
    CONSUMABLE,
    MATERIAL,
    QUEST,
    MISC
};

enum class ItemRarity {
    COMMON,
    UNCOMMON,
    RARE,
    EPIC,
    LEGENDARY
};

enum class WeaponType {
    SWORD,
    AXE,
    MACE,
    DAGGER,
    BOW,
    STAFF,
    WAND,
    NONE
};

enum class ArmorType {
    HELMET,
    CHESTPLATE,
    GAUNTLETS,
    GREAVES,
    BOOTS,
    SHIELD,
    NONE
};

class Item {
private:
    std::string name;
    std::string description;
    ItemType type;
    ItemRarity rarity;
    WeaponType weaponType;
    ArmorType armorType;
    
    // Stats
    stattype strength;
    stattype dexterity;
    stattype intelligence;
    welltype healthBonus;
    welltype manaBonus;
    
    // Combat stats
    stattype damage;
    stattype armor;
    stattype durability;
    stattype maxDurability;
    
    // Stacking
    stattype quantity;
    stattype maxStack;
    
    // Value
    stattype goldValue;
    
    // Requirements
    leveltype requiredLevel;
    stattype requiredStrength;
    stattype requiredDexterity;
    stattype requiredIntelligence;

public:
    // Constructors
    Item();
    Item(const std::string& name, const std::string& description, ItemType type);
    Item(const std::string& name, const std::string& description, ItemType type, ItemRarity rarity);
    
    // Getters
    std::string getName() const { return name; }
    std::string getDescription() const { return description; }
    ItemType getType() const { return type; }
    ItemRarity getRarity() const { return rarity; }
    WeaponType getWeaponType() const { return weaponType; }
    ArmorType getArmorType() const { return armorType; }
    
    // Stat getters
    stattype getStrength() const { return strength; }
    stattype getDexterity() const { return dexterity; }
    stattype getIntelligence() const { return intelligence; }
    welltype getHealthBonus() const { return healthBonus; }
    welltype getManaBonus() const { return manaBonus; }
    
    // Combat stat getters
    stattype getDamage() const { return damage; }
    stattype getArmor() const { return armor; }
    stattype getDurability() const { return durability; }
    stattype getMaxDurability() const { return maxDurability; }
    
    // Stacking getters
    stattype getQuantity() const { return quantity; }
    stattype getMaxStack() const { return maxStack; }
    
    // Value getters
    stattype getGoldValue() const { return goldValue; }
    
    // Requirement getters
    leveltype getRequiredLevel() const { return requiredLevel; }
    stattype getRequiredStrength() const { return requiredStrength; }
    stattype getRequiredDexterity() const { return requiredDexterity; }
    stattype getRequiredIntelligence() const { return requiredIntelligence; }
    
    // Setters
    void setName(const std::string& name) { this->name = name; }
    void setDescription(const std::string& description) { this->description = description; }
    void setType(ItemType type) { this->type = type; }
    void setRarity(ItemRarity rarity) { this->rarity = rarity; }
    void setWeaponType(WeaponType weaponType) { this->weaponType = weaponType; }
    void setArmorType(ArmorType armorType) { this->armorType = armorType; }
    
    // Stat setters
    void setStrength(stattype strength) { this->strength = strength; }
    void setDexterity(stattype dexterity) { this->dexterity = dexterity; }
    void setIntelligence(stattype intelligence) { this->intelligence = intelligence; }
    void setHealthBonus(welltype healthBonus) { this->healthBonus = healthBonus; }
    void setManaBonus(welltype manaBonus) { this->manaBonus = manaBonus; }
    
    // Combat stat setters
    void setDamage(stattype damage) { this->damage = damage; }
    void setArmor(stattype armor) { this->armor = armor; }
    void setDurability(stattype durability) { this->durability = durability; }
    void setMaxDurability(stattype maxDurability) { this->maxDurability = maxDurability; }
    
    // Stacking setters
    void setQuantity(stattype quantity) { this->quantity = quantity; }
    void setMaxStack(stattype maxStack) { this->maxStack = maxStack; }
    
    // Value setters
    void setGoldValue(stattype goldValue) { this->goldValue = goldValue; }
    
    // Requirement setters
    void setRequiredLevel(leveltype requiredLevel) { this->requiredLevel = requiredLevel; }
    void setRequiredStrength(stattype requiredStrength) { this->requiredStrength = requiredStrength; }
    void setRequiredDexterity(stattype requiredDexterity) { this->requiredDexterity = requiredDexterity; }
    void setRequiredIntelligence(stattype requiredIntelligence) { this->requiredIntelligence = requiredIntelligence; }
    
    // Utility methods
    bool isStackable() const;
    bool canStackWith(const Item& other) const;
    bool addToStack(stattype amount);
    bool removeFromStack(stattype amount);
    bool isBroken() const;
    void repair();
    void damageItem(stattype amount);
    
    // Factory methods for common items
    static Item createSword(const std::string& name, ItemRarity rarity = ItemRarity::COMMON);
    static Item createStaff(const std::string& name, ItemRarity rarity = ItemRarity::COMMON);
    static Item createBow(const std::string& name, ItemRarity rarity = ItemRarity::COMMON);
    static Item createArmor(const std::string& name, ArmorType armorType, ItemRarity rarity = ItemRarity::COMMON);
    static Item createPotion(const std::string& name, welltype healthRestore, welltype manaRestore);
    static Item createMaterial(const std::string& name, stattype maxStack = 99);
    
    // Display methods
    std::string getRarityString() const;
    std::string getTypeString() const;
    std::string getFullDescription() const;
    
    // Comparison operators
    bool operator==(const Item& other) const;
    bool operator!=(const Item& other) const;
};

#endif // ITEM_H
