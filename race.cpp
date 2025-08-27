#include "race.h"

// Constructor implementation
Race::Race(std::string name, std::string description, stattype strengthBonus, stattype dexterityBonus, 
           stattype intelligenceBonus, welltype healthBonus, welltype manaBonus, bool isPlayable, bool isHostile,
           welltype attackBonus, welltype defenseBonus, welltype speedBonus)
    : name(name), description(description), strengthBonus(strengthBonus), dexterityBonus(dexterityBonus),
      intelligenceBonus(intelligenceBonus), healthBonus(healthBonus), manaBonus(manaBonus), 
      isPlayable(isPlayable), isHostile(isHostile), attackBonus(attackBonus), defenseBonus(defenseBonus), 
      speedBonus(speedBonus) {
}

// Default constructor
Race::Race() : name(""), description(""), strengthBonus(0), dexterityBonus(0), intelligenceBonus(0),
               healthBonus(0), manaBonus(0), isPlayable(false), isHostile(false),
               attackBonus(0), defenseBonus(0), speedBonus(0) {
}

// Static factory methods for preset races
// name, description, strengthBonus, dexterityBonus, intelligenceBonus, healthBonus, manaBonus, isPlayable, isHostile, attackBonus, defenseBonus, speedBonus
Race Race::createHuman() {
    return Race("Human", "Adaptable and balanced", 2, 1, 1, 10, 5, true, false, 0, 0, 0);
}

Race Race::createElf() {
    return Race("Elf", "Graceful and magical", 1, 2, 3, 0, 15, true, false, 0, 0, 1);
}

Race Race::createDwarf() {
    return Race("Dwarf", "Strong and resilient", 3, 0, 1, 20, 0, true, false, 0, 2, 0);
}

Race Race::createGnome() {
    return Race("Gnome", "Small but intelligent", 1, 1, 4, 10, 20, true, false, 0, 0, 0);
}

Race Race::createHalfling() {
    return Race("Halfling", "Quick and nimble", 1, 3, 0, 0, 10, true, false, 0, 0, 2);
}

Race Race::createOrc() {
    return Race("Orc", "Strong and aggressive", 4, 0, 2, 15, 0, false, true, 2, 0, 0);
}

Race Race::createTroll() {
    return Race("Troll", "Massive and regenerating", 5, 1, 1, 30, 0, false, true, 3, 1, 0);
}

Race Race::createGoblin() {
    return Race("Goblin", "Small and sneaky", 1, 2, 0, 1, 5, false, true, 0, 0, 1);
}

Race Race::createKobold() {
    return Race("Kobold", "Tiny and cunning", 1, 3, 1, 0, 10, false, true, 0, 0, 2);
}

Race Race::createLizardman() {
    return Race("Lizardman", "Scaly and strong", 2, 1, 0, 15, 0, false, true, 1, 1, 0);
}

Race Race::createMinotaur() {
    return Race("Minotaur", "Powerful and intimidating", 6, 0, 0, 25, 0, false, true, 3, 0, 0);
}

// Monster-specific races
Race Race::createDragon() {
    return Race("Dragon", "Ancient and powerful", 8, 0, 4, 50, 30, false, true, 5, 3, 2);
}

Race Race::createUndead() {
    return Race("Undead", "Unholy and resilient", 1, 0, 2, 0, 20, false, true, 1, 2, 0);
}

Race Race::createDemon() {
    return Race("Demon", "Chaotic and magical", 3, 1, 3, 10, 25, false, true, 2, 1, 1);
}

Race Race::createBeast() {
    return Race("Beast", "Wild and instinctive", 2, 3, 0, 20, 0, false, true, 1, 0, 3);
}

// Getter implementations
std::string Race::getName() const { return name; }
std::string Race::getDescription() const { return description; }
stattype Race::getStrengthBonus() const { return strengthBonus; }
stattype Race::getDexterityBonus() const { return dexterityBonus; }
stattype Race::getIntelligenceBonus() const { return intelligenceBonus; }
welltype Race::getHealthBonus() const { return healthBonus; }
welltype Race::getManaBonus() const { return manaBonus; }
bool Race::getIsPlayable() const { return isPlayable; }
bool Race::getIsHostile() const { return isHostile; }
welltype Race::getAttackBonus() const { return attackBonus; }
welltype Race::getDefenseBonus() const { return defenseBonus; }
welltype Race::getSpeedBonus() const { return speedBonus; }
