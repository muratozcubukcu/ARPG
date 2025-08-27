#include "mob.h"
#include <algorithm>

// Constructor implementation
Mob::Mob(Race race)
    : race(race), isStunned(false), isSilenced(false), isRooted(false) {
    // Initialize with default stats based on race
    stattype baseStrength = race.getStrengthBonus();
    stattype baseDexterity = race.getDexterityBonus();
    stattype baseIntelligence = race.getIntelligenceBonus();
    welltype baseMaxHealth = race.getHealthBonus();
    welltype baseMaxMana = race.getManaBonus();
    
    stats = StatBlock(baseStrength, baseDexterity, baseIntelligence, baseMaxHealth, baseMaxMana);
}

// Basic getter methods
Race Mob::getRace() const { return race; }
StatBlock Mob::getStats() const { return stats; }

StatBlock& Mob::getStatsRef() { return stats; }

// Position method implementations
Position Mob::getPosition() const { return position; }

void Mob::setPosition(const Position& pos) {
    position = pos;
}

void Mob::setPosition(double x, double y, double z) {
    position.set(x, y, z);
}

void Mob::move(double deltaX, double deltaY, double deltaZ) {
    position.move(deltaX, deltaY, deltaZ);
}

double Mob::distanceTo(const Mob& other) const {
    return position.distanceTo(other.getPosition());
}

// Removed Character distance method to avoid circular dependency

double Mob::distanceTo(const Position& pos) const {
    return position.distanceTo(pos);
}

// Description methods
std::string Mob::getDescription() const {
    return race.getName();
}

std::string Mob::getFullDescription() const {
    return race.getName() + " - HP: " + std::to_string(stats.getHealth()) + "/" + std::to_string(stats.getMaxHealth());
}

// Combat methods
void Mob::damage(welltype amount) {
    stats.damage(amount);
}

void Mob::heal(welltype amount) {
    stats.heal(amount);
}

void Mob::restoreMana(welltype amount) {
    stats.restoreMana(amount);
}

void Mob::consumeMana(welltype amount) {
    stats.consumeMana(amount);
}

// Status effect management methods
void Mob::addStatusEffect(const StatusEffect& effect) {
    // Check if we already have this effect
    for (auto& existingEffect : statusEffects) {
        if (existingEffect.getName() == effect.getName() && existingEffect.canStackWith(effect)) {
            existingEffect.addStack(effect);
            return;
        }
    }
    
    // Add new effect
    statusEffects.push_back(effect);
    
    // Apply the effect immediately by setting up the target and calling applyEffect
    StatusEffect& newEffect = statusEffects.back();
    newEffect.setMobTarget(this);
    newEffect.applyEffect();
    
    // Debug: verify target is set
    std::cout << "DEBUG: Added effect '" << newEffect.getName() << "' to mob, target set: " << (newEffect.getMobTarget() == this ? "YES" : "NO") << std::endl;
    std::cout << "DEBUG: Mob " << getDescription() << " now has " << statusEffects.size() << " effects" << std::endl;
}

void Mob::removeStatusEffect(const std::string& effectName) {
    statusEffects.erase(
        std::remove_if(statusEffects.begin(), statusEffects.end(),
            [&effectName](const StatusEffect& effect) { 
                return effect.getName() == effectName; 
            }),
        statusEffects.end()
    );
}

void Mob::updateStatusEffects(float deltaTime) {
    std::cout << "DEBUG: Mob::updateStatusEffects ENTERED for " << getDescription() << std::endl;
    
    if (!statusEffects.empty()) {
        std::cout << "DEBUG: Mob::updateStatusEffects called with " << statusEffects.size() << " effects, deltaTime=" << deltaTime << std::endl;
    }
    
    for (auto& effect : statusEffects) {
        std::cout << "DEBUG: Updating effect '" << effect.getName() << "' on mob" << std::endl;
        effect.update(deltaTime);
    }
    
    // Remove expired effects
    statusEffects.erase(
        std::remove_if(statusEffects.begin(), statusEffects.end(),
            [](const StatusEffect& effect) { return effect.isExpired(); }),
        statusEffects.end()
    );
}

const std::vector<StatusEffect>& Mob::getStatusEffects() const {
    return statusEffects;
}

bool Mob::hasStatusEffect(const std::string& effectName) const {
    for (const auto& effect : statusEffects) {
        if (effect.getName() == effectName) {
            return true;
        }
    }
    return false;
}

// Stat modification methods for status effects
void Mob::modifyStrength(stattype amount) {
    stats.setStrength(stats.getStrength() + amount);
}

void Mob::modifyDexterity(stattype amount) {
    stats.setDexterity(stats.getDexterity() + amount);
}

void Mob::modifyIntelligence(stattype amount) {
    stats.setIntelligence(stats.getIntelligence() + amount);
}

void Mob::modifyMaxHealth(welltype amount) {
    stats.setMaxHealth(stats.getMaxHealth() + amount);
}

void Mob::modifyMaxMana(welltype amount) {
    stats.setMaxMana(stats.getMaxMana() + amount);
}

// Crowd control methods for status effects
void Mob::setStunned(bool stunned) {
    isStunned = stunned;
}

void Mob::setSilenced(bool silenced) {
    isSilenced = silenced;
}

void Mob::setRooted(bool rooted) {
    isRooted = rooted;
}
