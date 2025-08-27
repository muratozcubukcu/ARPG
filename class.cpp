#include "class.h"
#include "statblock.h"
#include "ability.h"
#include <algorithm>

// Constructor implementation
Class::Class(std::string name, stattype strength, stattype dexterity, stattype intelligence, 
             welltype maxHealth, welltype maxMana, welltype hpGrowth, welltype mpGrowth,
             stattype strGrowth, stattype dexGrowth, stattype intGrowth)
    : name(name), baseStrength(strength), baseDexterity(dexterity), baseIntelligence(intelligence),
      baseMaxHealth(maxHealth), baseMaxMana(maxMana), hpGrowth(hpGrowth), mpGrowth(mpGrowth),
      strGrowth(strGrowth), dexGrowth(dexGrowth), intGrowth(intGrowth) {
}

// Default constructor
Class::Class() : name(""), baseStrength(10), baseDexterity(10), baseIntelligence(10),
                 baseMaxHealth(50), baseMaxMana(25), hpGrowth(8), mpGrowth(4),
                 strGrowth(2), dexGrowth(2), intGrowth(2) {
    // Default balanced character: STR=10, DEX=10, INT=10, maxHP=50, maxMP=25
}

// Static factory methods
Class Class::createWarrior() {
    // name, strength, dexterity, intelligence, maxHealth, maxMana, hpGrowth, mpGrowth, strGrowth, dexGrowth, intGrowth, projectileSpeed, effectRadius
    Class warrior("Warrior", 15, 12, 8, 120, 15, 15, 3, 3, 2, 1);
    
    // Add abilities for different levels with full tick-based parameters
    warrior.addAbilityForLevel(1, Ability("Slash", "Basic sword attack", PHYSICAL, 12, 0, 0, 0, 2, ENEMY, DAMAGE, ACTIVE, INSTANT, SINGLE_TARGET, 0.0f, 0.0f));
    warrior.addAbilityForLevel(3, Ability("Shield Block", "Defensive stance", UTILITY, 0, 15, 0, 0, 0, SELF, BUFF, ACTIVE, INSTANT, SINGLE_TARGET, 0.0f, 0.0f));
    warrior.addAbilityForLevel(5, Ability("Power Strike", "Heavy damage attack", PHYSICAL, 25, 20, 1, 0.8, 3, ENEMY, DAMAGE, ACTIVE, INSTANT, SINGLE_TARGET, 0.0f, 0.0f));
    warrior.addAbilityForLevel(8, Ability("Charge", "Rush forward and attack", PHYSICAL, 20, 15, 2, 0.5, 8, ENEMY, DAMAGE, ACTIVE, INSTANT, SINGLE_TARGET, 0.0f, 0.0f));
    warrior.addAbilityForLevel(10, Ability("Whirlwind", "Spin attack hitting multiple enemies", PHYSICAL, 18, 30, 3, 1.5, 5, GROUND_TARGET, DAMAGE, ACTIVE, GROUND_CAST, CIRCLE, 0.0f, 4.0f));
    
    return warrior;
}

Class Class::createMage() {
    Class mage("Mage", 6, 8, 16, 60, 100, 5, 12, 1, 1, 3);
    
    // Add abilities for different levels with full tick-based parameters
    mage.addAbilityForLevel(1, Ability("Magic Bolt", "Basic magic projectile", MAGICAL, 15, 10, 1, 0.5, 25, PROJECTILE, DAMAGE, ACTIVE, PROJECTILE_CAST, SINGLE_TARGET, 20.0f, 0.5f));
    mage.addAbilityForLevel(3, Ability("Mana Shield", "Protective barrier", UTILITY, 0, 20, 0, 0, 0, SELF, BUFF, ACTIVE, INSTANT, SINGLE_TARGET, 0.0f, 0.0f));
    mage.addAbilityForLevel(5, Ability("Fireball", "Explosive fire projectile", MAGICAL, 30, 25, 2, 1.0, 30, PROJECTILE, DAMAGE, ACTIVE, PROJECTILE_CAST, SINGLE_TARGET, 15.0f, 3.0f));
    mage.addAbilityForLevel(8, Ability("Teleport", "Short range teleportation", UTILITY, 0, 25, 0, 0, 15, SELF, BUFF, ACTIVE, INSTANT, SINGLE_TARGET, 0.0f, 0.0f));
    mage.addAbilityForLevel(10, Ability("Lightning Storm", "Area lightning damage", MAGICAL, 50, 45, 3, 2.0, 20, GROUND_TARGET, DAMAGE, ACTIVE, GROUND_CAST, CIRCLE, 0.0f, 8.0f));
    
    return mage;
}

Class Class::createArcher() {
    Class archer("Archer", 8, 16, 10, 80, 30, 8, 6, 1, 3, 1);
    
    // Add abilities for different levels with full tick-based parameters
    archer.addAbilityForLevel(1, Ability("Quick Shot", "Fast arrow projectile", PHYSICAL, 10, 5, 0, 0.3, 35, PROJECTILE, DAMAGE, ACTIVE, PROJECTILE_CAST, SINGLE_TARGET, 35.0f, 0.3f));
    archer.addAbilityForLevel(3, Ability("Aimed Shot", "Precise arrow with bonus damage", PHYSICAL, 20, 12, 1, 1.2, 40, PROJECTILE, DAMAGE, ACTIVE, PROJECTILE_CAST, SINGLE_TARGET, 40.0f, 0.3f));
    archer.addAbilityForLevel(5, Ability("Multi-Shot", "Fire multiple arrows", PHYSICAL, 15, 18, 1, 0.8, 30, PROJECTILE, DAMAGE, ACTIVE, PROJECTILE_CAST, CONE, 30.0f, 2.0f));
    archer.addAbilityForLevel(8, Ability("Stealth", "Become invisible briefly", UTILITY, 0, 20, 0, 0, 0, SELF, BUFF, ACTIVE, INSTANT, SINGLE_TARGET, 0.0f, 0.0f));
    archer.addAbilityForLevel(10, Ability("Rain of Arrows", "Area arrow barrage", PHYSICAL, 25, 35, 2, 2.0, 25, GROUND_TARGET, DAMAGE, ACTIVE, GROUND_CAST, CIRCLE, 0.0f, 6.0f));
    
    return archer;
}

Class Class::createPaladin() {
    Class paladin("Paladin", 14, 10, 12, 110, 60, 12, 8, 2, 2, 2);
    
    // Add abilities for different levels
    paladin.addAbilityForLevel(1, Ability("Holy Strike", "Divine weapon attack", PHYSICAL, 0, 15, 0, 0, 0, ENEMY, DAMAGE, ACTIVE));
    paladin.addAbilityForLevel(3, Ability("Divine Protection", "Defensive blessing", UTILITY, 0, 20, 0, 0, 0, SELF, BUFF, ACTIVE));
    paladin.addAbilityForLevel(5, Ability("Smite", "Holy damage to enemy", MAGICAL, 0, 28, 0, 0, 0, ENEMY, DAMAGE, ACTIVE));
    paladin.addAbilityForLevel(8, Ability("Lay on Hands", "Heal self or ally", HEALING, 0, 25, 0, 0, 0, SELF, HEAL, ACTIVE));
    paladin.addAbilityForLevel(10, Ability("Divine Wrath", "Area holy damage", MAGICAL, 0, 40, 0, 0, 0, ENEMY, DAMAGE, ACTIVE));
    
    return paladin;
}

Class Class::createNone() {
    // name, strength, dexterity, intelligence, maxHealth, maxMana, hpGrowth, mpGrowth, strGrowth, dexGrowth, intGrowth
    Class none("None", 10, 10, 10, 50, 25, 8, 4, 2, 2, 2);
    return none;
}

// Getter implementations
stattype Class::getBaseStrength() const { return baseStrength; }
stattype Class::getBaseDexterity() const { return baseDexterity; }
stattype Class::getBaseIntelligence() const { return baseIntelligence; }
welltype Class::getBaseMaxHealth() const { return baseMaxHealth; }
welltype Class::getBaseMaxMana() const { return baseMaxMana; }
std::string Class::getName() const { return name; }

// Growth method
void Class::applyLevelUpGrowth(StatBlock& stats) const {
    stats.setMaxHealth(stats.getMaxHealth() + hpGrowth);
    stats.setMaxMana(stats.getMaxMana() + mpGrowth);
    stats.setStrength(stats.getStrength() + strGrowth);
    stats.setDexterity(stats.getDexterity() + dexGrowth);
    stats.setIntelligence(stats.getIntelligence() + intGrowth);
    
    // Restore health and mana to full after level up
    stats.setHealth(stats.getMaxHealth());
    stats.setMana(stats.getMaxMana());
}

// Ability methods
void Class::addAbilityForLevel(int level, const Ability& ability) {
    levelAbilities[level].push_back(ability);
}

std::vector<Ability> Class::getAbilitiesForLevel(int level) const {
    auto it = levelAbilities.find(level);
    if (it != levelAbilities.end()) {
        return it->second;
    }
    return std::vector<Ability>(); // Return empty vector if no abilities for this level
}

std::vector<Ability> Class::getAllAbilities() const {
    std::vector<Ability> allAbilities;
    for (const auto& level : levelAbilities) {
        allAbilities.insert(allAbilities.end(), level.second.begin(), level.second.end());
    }
    return allAbilities;
}
