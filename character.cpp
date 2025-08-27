#include "character.h"
#include "ability.h"
#include "mob.h"
#include <algorithm>

// Constructor implementation
Character::Character(std::string name, Race race, Class characterClass)
    : name(name), race(race), characterClass(characterClass), isStunned(false), isSilenced(false), isRooted(false) {
    
    // Calculate final stats by combining class base stats with race bonuses
    stattype finalStrength = characterClass.getBaseStrength() + race.getStrengthBonus();
    stattype finalDexterity = characterClass.getBaseDexterity() + race.getDexterityBonus();
    stattype finalIntelligence = characterClass.getBaseIntelligence() + race.getIntelligenceBonus();
    welltype finalMaxHealth = characterClass.getBaseMaxHealth() + race.getHealthBonus();
    welltype finalMaxMana = characterClass.getBaseMaxMana() + race.getManaBonus();
    
    // Create the final StatBlock with calculated values
    finalStats = StatBlock(finalStrength, finalDexterity, finalIntelligence, finalMaxHealth, finalMaxMana);
    
    // Add starting abilities for level 1
    std::vector<Ability> startingAbilities = characterClass.getAbilitiesForLevel(1);
    for (const auto& ability : startingAbilities) {
        abilities.push_back(ability);
    }
}

// Default constructor
Character::Character() : name(""), race(Race()), characterClass(Class()), isStunned(false), isSilenced(false), isRooted(false) {
    // Creates a default character with no name, default race/class, and default stats
}

// Getter implementations
std::string Character::getName() const { return name; }
Race Character::getRace() const { return race; }
Class Character::getCharacterClass() const { return characterClass; }
StatBlock Character::getStats() const { return finalStats; }

// Position method implementations
Position Character::getPosition() const { return position; }

void Character::setPosition(const Position& pos) {
    position = pos;
}

void Character::setPosition(double x, double y, double z) {
    position.set(x, y, z);
}

void Character::move(double deltaX, double deltaY, double deltaZ) {
    position.move(deltaX, deltaY, deltaZ);
}

double Character::distanceTo(const Character& other) const {
    return position.distanceTo(other.getPosition());
}

double Character::distanceTo(const Position& pos) const {
    return position.distanceTo(pos);
}

// Ability methods
const std::vector<Ability>& Character::getAbilities() const { return abilities; }

void Character::addAbility(const Ability& ability) {
    abilities.push_back(ability);
}

std::vector<Ability> Character::getNewAbilitiesForLevel(int level) const {
    return characterClass.getAbilitiesForLevel(level);
}

// Utility methods
void Character::addExp(exptype amount) {
    finalStats.addExp(amount);
    while (canLevelUp()) {
        levelUp();
    }
}

bool Character::canLevelUp() const {
    return finalStats.canLevelUp();
}

void Character::levelUp() {
    if (canLevelUp()) {
        finalStats.setExp(finalStats.getExp() - (finalStats.getLevel() * 100));
        int newLevel = finalStats.getLevel() + 1;
        finalStats.setLevel(newLevel);
        
        // Get new abilities for this level
        std::vector<Ability> newAbilities = characterClass.getAbilitiesForLevel(newLevel);
        for (const auto& ability : newAbilities) {
            abilities.push_back(ability);
        }
        
        // Apply class-specific growth
        characterClass.applyLevelUpGrowth(finalStats);
    }
}

void Character::heal(welltype amount) {
    finalStats.heal(amount);
}

void Character::damage(welltype amount) {
    finalStats.damage(amount);
}

void Character::restoreMana(welltype amount) {
    finalStats.restoreMana(amount);
}

void Character::consumeMana(welltype amount) {
    finalStats.consumeMana(amount);
}

// Character info
std::string Character::getFullDescription() const {
    return name + " the " + race.getName() + " " + characterClass.getName();
}

// Status effect management methods
void Character::addStatusEffect(const StatusEffect& effect) {
    // Check if we already have this effect
    for (auto& existingEffect : statusEffects) {
        if (existingEffect.getName() == effect.getName() && existingEffect.canStackWith(effect)) {
            std::cout << "DEBUG: Found existing effect '" << effect.getName() << "', stacking..." << std::endl;
            existingEffect.addStack(effect);
            return;
        }
    }
    
    std::cout << "DEBUG: Adding new effect '" << effect.getName() << "'" << std::endl;
    
    // Add new effect
    statusEffects.push_back(effect);
    
    // Apply the effect immediately by setting up the target and calling applyEffect
    StatusEffect& newEffect = statusEffects.back();
    newEffect.setCharacterTarget(this);
    newEffect.applyEffect();
}

void Character::removeStatusEffect(const std::string& effectName) {
    statusEffects.erase(
        std::remove_if(statusEffects.begin(), statusEffects.end(),
            [&effectName](const StatusEffect& effect) { 
                return effect.getName() == effectName; 
            }),
        statusEffects.end()
    );
}

void Character::updateStatusEffects(float deltaTime) {
    for (auto& effect : statusEffects) {
        effect.update(deltaTime);
    }
    
    // Remove expired effects
    statusEffects.erase(
        std::remove_if(statusEffects.begin(), statusEffects.end(),
            [](const StatusEffect& effect) { return effect.isExpired(); }),
        statusEffects.end()
    );
}

const std::vector<StatusEffect>& Character::getStatusEffects() const {
    return statusEffects;
}

bool Character::hasStatusEffect(const std::string& effectName) const {
    for (const auto& effect : statusEffects) {
        if (effect.getName() == effectName) {
            return true;
        }
    }
    return false;
}

// Ability usage methods
bool Character::useAbility(const Ability& ability, Character& target) {
    // Check if we have this ability
    bool hasAbility = false;
    for (const auto& ab : abilities) {
        if (ab.getName() == ability.getName()) {
            hasAbility = true;
            break;
        }
    }
    
    if (!hasAbility) {
        std::cout << name << " doesn't know the ability " << ability.getName() << "!" << std::endl;
        return false;
    }
    
    // Use the ability
    ability.cast(*this, target);
    return true;
}

bool Character::useAbility(const Ability& ability, Mob& target) {
    // Check if we have this ability
    bool hasAbility = false;
    for (const auto& ab : abilities) {
        if (ab.getName() == ability.getName()) {
            hasAbility = true;
            break;
        }
    }
    
    if (!hasAbility) {
        std::cout << name << " doesn't know the ability " << ability.getName() << "!" << std::endl;
        return false;
    }
    
    // Use the ability
    ability.cast(*this, target);
    return true;
}

// Stat modification methods for status effects
void Character::modifyStrength(stattype amount) {
    stattype oldStrength = finalStats.getStrength();
    stattype newStrength = oldStrength + amount;
    std::cout << "DEBUG: modifyStrength called - Old: " << oldStrength << ", Adding: " << amount << ", New: " << newStrength << std::endl;
    finalStats.setStrength(newStrength);
    std::cout << "DEBUG: Strength after modification: " << finalStats.getStrength() << std::endl;
}

void Character::modifyDexterity(stattype amount) {
    finalStats.setDexterity(finalStats.getDexterity() + amount);
}

void Character::modifyIntelligence(stattype amount) {
    finalStats.setIntelligence(finalStats.getIntelligence() + amount);
}

void Character::modifyMaxHealth(welltype amount) {
    finalStats.setMaxHealth(finalStats.getMaxHealth() + amount);
}

void Character::modifyMaxMana(welltype amount) {
    finalStats.setMaxMana(finalStats.getMaxMana() + amount);
}

// Crowd control methods for status effects
void Character::setStunned(bool stunned) {
    isStunned = stunned;
}

void Character::setSilenced(bool silenced) {
    isSilenced = silenced;
}

void Character::setRooted(bool rooted) {
    isRooted = rooted;
}