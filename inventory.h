#ifndef INVENTORY_H
#define INVENTORY_H

#include "item.h"
#include "types.h"
#include <vector>
#include <map>
#include <string>

enum class EquipmentSlot {
    WEAPON_MAIN_HAND,
    WEAPON_OFF_HAND,
    ARMOR_HEAD,
    ARMOR_CHEST,
    ARMOR_HANDS,
    ARMOR_LEGS,
    ARMOR_FEET,
    ARMOR_SHIELD,
    ACCESSORY_1,
    ACCESSORY_2,
    NONE
};

class Inventory {
public:
    Inventory();
    ~Inventory();

    // Basic inventory operations
    bool addItem(const Item& item);
    bool removeItem(const std::string& itemName, stattype quantity = 1);
    bool hasItem(const std::string& itemName) const;
    stattype getItemCount(const std::string& itemName) const;
    void clear();
    
    // Equipment management
    bool equipItem(const Item& item, EquipmentSlot slot);
    bool unequipItem(EquipmentSlot slot);
    Item* getEquippedItem(EquipmentSlot slot);
    bool isSlotOccupied(EquipmentSlot slot) const;
    
    // Inventory queries
    std::vector<Item> getAllItems() const;
    std::vector<Item> getItemsByType(ItemType type) const;
    std::vector<Item> getItemsByRarity(ItemRarity rarity) const;
    std::vector<Item> getEquippedItems() const;
    
    // Inventory capacity
    stattype getMaxSlots() const;
    stattype getUsedSlots() const;
    stattype getFreeSlots() const;
    void setMaxSlots(stattype maxSlots);
    bool isFull() const;
    
    // Item stacking and management
    bool stackItem(const Item& item);
    bool splitStack(const std::string& itemName, stattype splitAmount);
    bool mergeStacks(const std::string& itemName1, const std::string& itemName2);
    
    // Utility methods
    void print() const;
    void printEquipment() const;
    std::string getInventorySummary() const;
    
    // Equipment bonus calculations
    stattype getTotalStrengthBonus() const;
    stattype getTotalDexterityBonus() const;
    stattype getTotalIntelligenceBonus() const;
    welltype getTotalHealthBonus() const;
    welltype getTotalManaBonus() const;
    stattype getTotalDamageBonus() const;
    stattype getTotalArmorBonus() const;
    
    // Item search and filtering
    std::vector<Item> searchItems(const std::string& searchTerm) const;
    std::vector<Item> getItemsByLevel(leveltype minLevel, leveltype maxLevel) const;
    
    // Inventory persistence (for save/load)
    std::string serialize() const;
    bool deserialize(const std::string& data);

private:
    std::vector<Item> items;
    std::map<EquipmentSlot, Item> equippedItems;
    stattype maxSlots;
    
    // Helper methods
    bool canAddItem(const Item& item) const;
    bool tryStackWithExisting(const Item& item);
    EquipmentSlot getDefaultSlotForItem(const Item& item) const;
    bool isValidSlotForItem(const Item& item, EquipmentSlot slot) const;
    void updateEquipmentStats();
};

#endif // INVENTORY_H