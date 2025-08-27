#include "inventory.h"
#include "character.h"
#include "race.h"
#include "class.h"
#include <iostream>

int main() {
    std::cout << "=== RPG Inventory System Test ===" << std::endl;
    
    // Create a character
    Character player("TestPlayer", Race::createHuman(), Class::createWarrior());
    
    std::cout << "\n=== Initial Character Stats ===" << std::endl;
    std::cout << "Name: " << player.getName() << std::endl;
    std::cout << "Race: " << player.getRace().getName() << std::endl;
    std::cout << "Class: " << player.getCharacterClass().getName() << std::endl;
    std::cout << "Strength: " << player.getStats().getStrength() << std::endl;
    std::cout << "Health: " << player.getStats().getMaxHealth() << std::endl;
    
    std::cout << "\n=== Testing Item Creation ===" << std::endl;
    
    // Create various items
    Item sword = Item::createSword("Iron Sword", ItemRarity::COMMON);
    Item rareSword = Item::createSword("Steel Greatsword", ItemRarity::RARE);
    Item helmet = Item::createArmor("Leather Helmet", ArmorType::HELMET, ItemRarity::COMMON);
    Item chestplate = Item::createArmor("Iron Chestplate", ArmorType::CHESTPLATE, ItemRarity::UNCOMMON);
    Item healthPotion = Item::createPotion("Health Potion", 50, 0);
    Item manaPotion = Item::createPotion("Mana Potion", 0, 30);
    Item ironOre = Item::createMaterial("Iron Ore", 99);
    
    std::cout << "Created items:" << std::endl;
    std::cout << "- " << sword.getName() << " (Damage: " << sword.getDamage() << ")" << std::endl;
    std::cout << "- " << rareSword.getName() << " (Damage: " << rareSword.getDamage() << ")" << std::endl;
    std::cout << "- " << helmet.getName() << " (Armor: " << helmet.getArmor() << ")" << std::endl;
    std::cout << "- " << chestplate.getName() << " (Armor: " << chestplate.getArmor() << ")" << std::endl;
    std::cout << "- " << healthPotion.getName() << " (Health: +" << healthPotion.getHealthBonus() << ")" << std::endl;
    std::cout << "- " << manaPotion.getName() << " (Mana: +" << manaPotion.getManaBonus() << ")" << std::endl;
    std::cout << "- " << ironOre.getName() << " (Stackable: " << (ironOre.isStackable() ? "Yes" : "No") << ")" << std::endl;
    
    std::cout << "\n=== Testing Inventory Operations ===" << std::endl;
    
    // Add items to inventory
    std::cout << "Adding items to inventory..." << std::endl;
    player.addItemToInventory(sword);
    player.addItemToInventory(rareSword);
    player.addItemToInventory(helmet);
    player.addItemToInventory(chestplate);
    player.addItemToInventory(healthPotion);
    player.addItemToInventory(manaPotion);
    player.addItemToInventory(ironOre);
    
    // Test stacking
    std::cout << "Adding more iron ore to test stacking..." << std::endl;
    Item ironOre2 = Item::createMaterial("Iron Ore", 99);
    ironOre2.setQuantity(5);
    player.addItemToInventory(ironOre2);
    
    std::cout << "Iron ore count: " << player.getItemCount("Iron Ore") << std::endl;
    
    // Display inventory
    std::cout << "\n=== Current Inventory ===" << std::endl;
    player.getInventory().print();
    
    std::cout << "\n=== Testing Equipment System ===" << std::endl;
    
    // Equip items
    std::cout << "Equipping sword to main hand..." << std::endl;
    if (player.equipItem(sword, EquipmentSlot::WEAPON_MAIN_HAND)) {
        std::cout << "Successfully equipped " << sword.getName() << "!" << std::endl;
    }
    
    std::cout << "Equipping helmet..." << std::endl;
    if (player.equipItem(helmet, EquipmentSlot::ARMOR_HEAD)) {
        std::cout << "Successfully equipped " << helmet.getName() << "!" << std::endl;
    }
    
    std::cout << "Equipping chestplate..." << std::endl;
    if (player.equipItem(chestplate, EquipmentSlot::ARMOR_CHEST)) {
        std::cout << "Successfully equipped " << chestplate.getName() << "!" << std::endl;
    }
    
    // Show updated stats
    std::cout << "\n=== Updated Character Stats ===" << std::endl;
    std::cout << "Strength: " << player.getStats().getStrength() << std::endl;
    std::cout << "Health: " << player.getStats().getMaxHealth() << std::endl;
    std::cout << "Armor: " << player.getStats().getArmor() << std::endl;
    
    // Show equipped items
    std::cout << "\n=== Equipped Items ===" << std::endl;
    player.getInventory().printEquipment();
    
    // Show updated inventory
    std::cout << "\n=== Updated Inventory ===" << std::endl;
    player.getInventory().print();
    
    std::cout << "\n=== Testing Item Search and Filtering ===" << std::endl;
    
    // Search for items
    std::vector<Item> weapons = player.getInventory().getItemsByType(ItemType::WEAPON);
    std::cout << "Weapons in inventory: " << weapons.size() << std::endl;
    
    std::vector<Item> rareItems = player.getInventory().getItemsByRarity(ItemRarity::RARE);
    std::cout << "Rare items in inventory: " << rareItems.size() << std::endl;
    
    std::vector<Item> searchResults = player.getInventory().searchItems("Iron");
    std::cout << "Items containing 'Iron': " << searchResults.size() << std::endl;
    
    std::cout << "\n=== Testing Item Removal ===" << std::endl;
    
    // Remove some items
    std::cout << "Removing 2 iron ore..." << std::endl;
    if (player.removeItemFromInventory("Iron Ore", 2)) {
        std::cout << "Successfully removed 2 iron ore!" << std::endl;
        std::cout << "Remaining iron ore: " << player.getItemCount("Iron Ore") << std::endl;
    }
    
    std::cout << "\n=== Final Inventory ===" << std::endl;
    player.getInventory().print();
    
    std::cout << "\n=== Inventory Summary ===" << std::endl;
    std::cout << player.getInventory().getInventorySummary();
    
    std::cout << "\n=== Testing Unequip ===" << std::endl;
    
    // Unequip helmet
    std::cout << "Unequipping helmet..." << std::endl;
    if (player.unequipItem(EquipmentSlot::ARMOR_HEAD)) {
        std::cout << "Successfully unequipped helmet!" << std::endl;
    }
    
    // Show final state
    std::cout << "\n=== Final Character Stats ===" << std::endl;
    std::cout << "Strength: " << player.getStats().getStrength() << std::endl;
    std::cout << "Health: " << player.getStats().getMaxHealth() << std::endl;
    
    std::cout << "\n=== Final Equipment ===" << std::endl;
    player.getInventory().printEquipment();
    
    std::cout << "\n=== Final Inventory ===" << std::endl;
    player.getInventory().print();
    
    std::cout << "\n=== Inventory System Test Complete! ===" << std::endl;
    
    return 0;
}
