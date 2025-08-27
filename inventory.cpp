#include "inventory.h"
#include <iostream>
#include <algorithm>
#include <sstream>
#include <iomanip>

Inventory::Inventory() : maxSlots(30) {
}

Inventory::~Inventory() {
}

// Basic inventory operations
bool Inventory::addItem(const Item& item) {
    if (!canAddItem(item)) {
        return false;
    }
    
    // Try to stack with existing items first
    if (tryStackWithExisting(item)) {
        return true;
    }
    
    // Add as new item
    items.push_back(item);
    return true;
}

bool Inventory::removeItem(const std::string& itemName, stattype quantity) {
    for (auto it = items.begin(); it != items.end(); ++it) {
        if (it->getName() == itemName) {
            if (it->getQuantity() <= quantity) {
                // Remove entire stack
                items.erase(it);
            } else {
                // Remove partial stack
                it->removeFromStack(quantity);
            }
            return true;
        }
    }
    return false;
}

bool Inventory::hasItem(const std::string& itemName) const {
    return std::any_of(items.begin(), items.end(), 
        [&itemName](const Item& item) { return item.getName() == itemName; });
}

stattype Inventory::getItemCount(const std::string& itemName) const {
    stattype total = 0;
    for (const auto& item : items) {
        if (item.getName() == itemName) {
            total += item.getQuantity();
        }
    }
    return total;
}

void Inventory::clear() {
    items.clear();
    equippedItems.clear();
}

// Equipment management
bool Inventory::equipItem(const Item& item, EquipmentSlot slot) {
    if (!isValidSlotForItem(item, slot)) {
        return false;
    }
    
    // Unequip current item in slot if any
    if (isSlotOccupied(slot)) {
        unequipItem(slot);
    }
    
    // Find and remove item from inventory
    for (auto it = items.begin(); it != items.end(); ++it) {
        if (it->getName() == item.getName()) {
            equippedItems[slot] = *it;
            items.erase(it);
            return true;
        }
    }
    
    return false;
}

bool Inventory::unequipItem(EquipmentSlot slot) {
    if (!isSlotOccupied(slot)) {
        return false;
    }
    
    Item unequippedItem = equippedItems[slot];
    equippedItems.erase(slot);
    
    // Try to add back to inventory
    if (addItem(unequippedItem)) {
        return true;
    } else {
        // If inventory is full, drop the item (could implement a drop system)
        std::cout << "Warning: Could not unequip item, inventory full!" << std::endl;
        return false;
    }
}

Item* Inventory::getEquippedItem(EquipmentSlot slot) {
    auto it = equippedItems.find(slot);
    if (it != equippedItems.end()) {
        return &(it->second);
    }
    return nullptr;
}

bool Inventory::isSlotOccupied(EquipmentSlot slot) const {
    return equippedItems.find(slot) != equippedItems.end();
}

// Inventory queries
std::vector<Item> Inventory::getAllItems() const {
    return items;
}

std::vector<Item> Inventory::getItemsByType(ItemType type) const {
    std::vector<Item> result;
    for (const auto& item : items) {
        if (item.getType() == type) {
            result.push_back(item);
        }
    }
    return result;
}

std::vector<Item> Inventory::getItemsByRarity(ItemRarity rarity) const {
    std::vector<Item> result;
    for (const auto& item : items) {
        if (item.getRarity() == rarity) {
            result.push_back(item);
        }
    }
    return result;
}

std::vector<Item> Inventory::getEquippedItems() const {
    std::vector<Item> result;
    for (const auto& pair : equippedItems) {
        result.push_back(pair.second);
    }
    return result;
}

// Inventory capacity
stattype Inventory::getMaxSlots() const {
    return maxSlots;
}

stattype Inventory::getUsedSlots() const {
    return items.size();
}

stattype Inventory::getFreeSlots() const {
    return maxSlots - items.size();
}

void Inventory::setMaxSlots(stattype maxSlots) {
    this->maxSlots = maxSlots;
}

bool Inventory::isFull() const {
    return items.size() >= maxSlots;
}

// Item stacking and management
bool Inventory::stackItem(const Item& item) {
    return tryStackWithExisting(item);
}

bool Inventory::splitStack(const std::string& itemName, stattype splitAmount) {
    for (auto it = items.begin(); it != items.end(); ++it) {
        if (it->getName() == itemName && it->isStackable() && it->getQuantity() > splitAmount) {
            Item newStack = *it;
            newStack.setQuantity(splitAmount);
            it->removeFromStack(splitAmount);
            items.push_back(newStack);
            return true;
        }
    }
    return false;
}

bool Inventory::mergeStacks(const std::string& itemName1, const std::string& itemName2) {
    Item* stack1 = nullptr;
    Item* stack2 = nullptr;
    
    // Find both stacks
    for (auto& item : items) {
        if (item.getName() == itemName1 && !stack1) {
            stack1 = &item;
        } else if (item.getName() == itemName2 && !stack2) {
            stack2 = &item;
        }
    }
    
    if (stack1 && stack2 && stack1->canStackWith(*stack2)) {
        stattype totalQuantity = stack1->getQuantity() + stack2->getQuantity();
        if (totalQuantity <= stack1->getMaxStack()) {
            stack1->setQuantity(totalQuantity);
            // Remove stack2
            items.erase(std::remove(items.begin(), items.end(), *stack2), items.end());
            return true;
        }
    }
    
    return false;
}

// Utility methods
void Inventory::print() const {
    std::cout << "\n=== INVENTORY (" << getUsedSlots() << "/" << getMaxSlots() << " slots) ===" << std::endl;
    
    if (items.empty()) {
        std::cout << "Inventory is empty." << std::endl;
        return;
    }
    
    for (size_t i = 0; i < items.size(); ++i) {
        std::cout << "[" << i + 1 << "] " << items[i].getName();
        if (items[i].isStackable()) {
            std::cout << " (x" << items[i].getQuantity() << ")";
        }
        std::cout << " - " << items[i].getRarityString() << " " << items[i].getTypeString();
        std::cout << std::endl;
    }
}

void Inventory::printEquipment() const {
    std::cout << "\n=== EQUIPPED ITEMS ===" << std::endl;
    
    if (equippedItems.empty()) {
        std::cout << "No items equipped." << std::endl;
        return;
    }
    
    for (const auto& pair : equippedItems) {
        EquipmentSlot slot = pair.first;
        const Item& item = pair.second;
        
        std::string slotName;
        switch (slot) {
            case EquipmentSlot::WEAPON_MAIN_HAND: slotName = "Main Hand"; break;
            case EquipmentSlot::WEAPON_OFF_HAND: slotName = "Off Hand"; break;
            case EquipmentSlot::ARMOR_HEAD: slotName = "Head"; break;
            case EquipmentSlot::ARMOR_CHEST: slotName = "Chest"; break;
            case EquipmentSlot::ARMOR_HANDS: slotName = "Hands"; break;
            case EquipmentSlot::ARMOR_LEGS: slotName = "Legs"; break;
            case EquipmentSlot::ARMOR_FEET: slotName = "Feet"; break;
            case EquipmentSlot::ARMOR_SHIELD: slotName = "Shield"; break;
            case EquipmentSlot::ACCESSORY_1: slotName = "Accessory 1"; break;
            case EquipmentSlot::ACCESSORY_2: slotName = "Accessory 2"; break;
            default: slotName = "Unknown"; break;
        }
        
        std::cout << slotName << ": " << item.getName() << " (" << item.getRarityString() << ")" << std::endl;
    }
}

std::string Inventory::getInventorySummary() const {
    std::stringstream ss;
    ss << "Inventory: " << getUsedSlots() << "/" << getMaxSlots() << " slots used\n";
    ss << "Items: " << items.size() << "\n";
    ss << "Equipped: " << equippedItems.size() << " items\n";
    
    // Count items by type
    std::map<ItemType, int> typeCounts;
    for (const auto& item : items) {
        typeCounts[item.getType()]++;
    }
    
    ss << "By type: ";
    for (const auto& pair : typeCounts) {
        ss << pair.second << " ";
        switch (pair.first) {
            case ItemType::WEAPON: ss << "Weapons"; break;
            case ItemType::ARMOR: ss << "Armor"; break;
            case ItemType::CONSUMABLE: ss << "Consumables"; break;
            case ItemType::MATERIAL: ss << "Materials"; break;
            case ItemType::QUEST: ss << "Quest Items"; break;
            case ItemType::MISC: ss << "Misc"; break;
            default: ss << "Unknown"; break;
        }
        ss << ", ";
    }
    
    return ss.str();
}

// Equipment bonus calculations
stattype Inventory::getTotalStrengthBonus() const {
    stattype total = 0;
    for (const auto& pair : equippedItems) {
        total += pair.second.getStrength();
    }
    return total;
}

stattype Inventory::getTotalDexterityBonus() const {
    stattype total = 0;
    for (const auto& pair : equippedItems) {
        total += pair.second.getDexterity();
    }
    return total;
}

stattype Inventory::getTotalIntelligenceBonus() const {
    stattype total = 0;
    for (const auto& pair : equippedItems) {
        total += pair.second.getIntelligence();
    }
    return total;
}

welltype Inventory::getTotalHealthBonus() const {
    welltype total = 0;
    for (const auto& pair : equippedItems) {
        total += pair.second.getHealthBonus();
    }
    return total;
}

welltype Inventory::getTotalManaBonus() const {
    welltype total = 0;
    for (const auto& pair : equippedItems) {
        total += pair.second.getManaBonus();
    }
    return total;
}

stattype Inventory::getTotalDamageBonus() const {
    stattype total = 0;
    for (const auto& pair : equippedItems) {
        total += pair.second.getDamage();
    }
    return total;
}

stattype Inventory::getTotalArmorBonus() const {
    stattype total = 0;
    for (const auto& pair : equippedItems) {
        total += pair.second.getArmor();
    }
    return total;
}

// Item search and filtering
std::vector<Item> Inventory::searchItems(const std::string& searchTerm) const {
    std::vector<Item> result;
    std::string lowerSearch = searchTerm;
    std::transform(lowerSearch.begin(), lowerSearch.end(), lowerSearch.begin(), ::tolower);
    
    for (const auto& item : items) {
        std::string itemName = item.getName();
        std::transform(itemName.begin(), itemName.end(), itemName.begin(), ::tolower);
        
        if (itemName.find(lowerSearch) != std::string::npos) {
            result.push_back(item);
        }
    }
    
    return result;
}

std::vector<Item> Inventory::getItemsByLevel(leveltype minLevel, leveltype maxLevel) const {
    std::vector<Item> result;
    for (const auto& item : items) {
        leveltype itemLevel = item.getRequiredLevel();
        if (itemLevel >= minLevel && itemLevel <= maxLevel) {
            result.push_back(item);
        }
    }
    return result;
}

// Inventory persistence
std::string Inventory::serialize() const {
    std::stringstream ss;
    // Simple serialization - could be enhanced with proper format
    ss << "INVENTORY_DATA\n";
    ss << "MAX_SLOTS:" << maxSlots << "\n";
    ss << "ITEMS_COUNT:" << items.size() << "\n";
    
    for (const auto& item : items) {
        ss << "ITEM:" << item.getName() << ":" << static_cast<int>(item.getType()) << ":" << item.getQuantity() << "\n";
    }
    
    ss << "EQUIPPED_COUNT:" << equippedItems.size() << "\n";
    for (const auto& pair : equippedItems) {
        ss << "EQUIPPED:" << static_cast<int>(pair.first) << ":" << pair.second.getName() << "\n";
    }
    
    return ss.str();
}

bool Inventory::deserialize(const std::string& data) {
    // Simple deserialization - could be enhanced with proper parsing
    std::istringstream iss(data);
    std::string line;
    
    clear();
    
    while (std::getline(iss, line)) {
        if (line.find("MAX_SLOTS:") == 0) {
            std::string value = line.substr(11);
            maxSlots = std::stoi(value);
        }
        // Could add more parsing logic here
    }
    
    return true;
}

// Helper methods
bool Inventory::canAddItem(const Item& item) const {
    if (isFull() && !item.isStackable()) {
        return false;
    }
    return true;
}

bool Inventory::tryStackWithExisting(const Item& item) {
    if (!item.isStackable()) {
        return false;
    }
    
    for (auto& existingItem : items) {
        if (existingItem.canStackWith(item)) {
            if (existingItem.addToStack(item.getQuantity())) {
                return true;
            }
        }
    }
    
    return false;
}

EquipmentSlot Inventory::getDefaultSlotForItem(const Item& item) const {
    switch (item.getType()) {
        case ItemType::WEAPON:
            if (item.getWeaponType() == WeaponType::SWORD || 
                item.getWeaponType() == WeaponType::AXE || 
                item.getWeaponType() == WeaponType::MACE ||
                item.getWeaponType() == WeaponType::DAGGER) {
                return EquipmentSlot::WEAPON_MAIN_HAND;
            } else if (item.getWeaponType() == WeaponType::STAFF || 
                       item.getWeaponType() == WeaponType::WAND) {
                return EquipmentSlot::WEAPON_MAIN_HAND;
            }
            break;
        case ItemType::ARMOR:
            switch (item.getArmorType()) {
                case ArmorType::HELMET: return EquipmentSlot::ARMOR_HEAD;
                case ArmorType::CHESTPLATE: return EquipmentSlot::ARMOR_CHEST;
                case ArmorType::GAUNTLETS: return EquipmentSlot::ARMOR_HANDS;
                case ArmorType::GREAVES: return EquipmentSlot::ARMOR_LEGS;
                case ArmorType::BOOTS: return EquipmentSlot::ARMOR_FEET;
                case ArmorType::SHIELD: return EquipmentSlot::ARMOR_SHIELD;
                default: break;
            }
            break;
        default:
            break;
    }
    
    return EquipmentSlot::NONE;
}

bool Inventory::isValidSlotForItem(const Item& item, EquipmentSlot slot) const {
    switch (slot) {
        case EquipmentSlot::WEAPON_MAIN_HAND:
        case EquipmentSlot::WEAPON_OFF_HAND:
            return item.getType() == ItemType::WEAPON;
        case EquipmentSlot::ARMOR_HEAD:
            return item.getType() == ItemType::ARMOR && item.getArmorType() == ArmorType::HELMET;
        case EquipmentSlot::ARMOR_CHEST:
            return item.getType() == ItemType::ARMOR && item.getArmorType() == ArmorType::CHESTPLATE;
        case EquipmentSlot::ARMOR_HANDS:
            return item.getType() == ItemType::ARMOR && item.getArmorType() == ArmorType::GAUNTLETS;
        case EquipmentSlot::ARMOR_LEGS:
            return item.getType() == ItemType::ARMOR && item.getArmorType() == ArmorType::GREAVES;
        case EquipmentSlot::ARMOR_FEET:
            return item.getType() == ItemType::ARMOR && item.getArmorType() == ArmorType::BOOTS;
        case EquipmentSlot::ARMOR_SHIELD:
            return item.getType() == ItemType::ARMOR && item.getArmorType() == ArmorType::SHIELD;
        case EquipmentSlot::ACCESSORY_1:
        case EquipmentSlot::ACCESSORY_2:
            return item.getType() == ItemType::MISC;
        default:
            return false;
    }
}

void Inventory::updateEquipmentStats() {
    // This method could be used to recalculate character stats when equipment changes
    // For now, it's a placeholder for future implementation
}


