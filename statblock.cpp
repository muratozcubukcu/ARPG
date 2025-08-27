#include "statblock.h"

// Constructor implementations
StatBlock::StatBlock(stattype strength, stattype dexterity, stattype intelligence, 
                     welltype health, welltype mana, leveltype level, exptype exp)
    : strength(strength), dexterity(dexterity), intelligence(intelligence),
      health(health), mana(mana), level(level), exp(exp), maxHealth(health), maxMana(mana),
      movementSpeed(1.0f), attackSpeed(1.0f), damageMultiplier(1.0f) {
}

StatBlock::StatBlock(stattype strength, stattype dexterity, stattype intelligence, 
                     welltype maxHealth, welltype maxMana)
    : strength(strength), dexterity(dexterity), intelligence(intelligence),
      health(maxHealth), mana(maxMana), level(1), exp(0), maxHealth(maxHealth), maxMana(maxMana),
      movementSpeed(1.0f), attackSpeed(1.0f), damageMultiplier(1.0f) {
}

StatBlock::StatBlock() : strength(10), dexterity(10), intelligence(10),
                         health(50), mana(25), level(1), exp(0), maxHealth(50), maxMana(25),
                         movementSpeed(1.0f), attackSpeed(1.0f), damageMultiplier(1.0f) {
}

// Getter implementations
stattype StatBlock::getStrength() const { return strength; }
stattype StatBlock::getDexterity() const { return dexterity; }
stattype StatBlock::getIntelligence() const { return intelligence; }
welltype StatBlock::getHealth() const { return health; }
welltype StatBlock::getMana() const { return mana; }
leveltype StatBlock::getLevel() const { return level; }
exptype StatBlock::getExp() const { return exp; }
welltype StatBlock::getMaxHealth() const { return maxHealth; }
welltype StatBlock::getMaxMana() const { return maxMana; }

// New getter implementations
float StatBlock::getMovementSpeed() const { return movementSpeed; }
float StatBlock::getAttackSpeed() const { return attackSpeed; }
float StatBlock::getDamageMultiplier() const { return damageMultiplier; }

// Setter implementations
void StatBlock::setStrength(stattype strength) { this->strength = strength; }
void StatBlock::setDexterity(stattype dexterity) { this->dexterity = dexterity; }
void StatBlock::setIntelligence(stattype intelligence) { this->intelligence = intelligence; }
void StatBlock::setHealth(welltype health) { this->health = health; }
void StatBlock::setMana(welltype mana) { this->mana = mana; }
void StatBlock::setLevel(leveltype level) { this->level = level; }
void StatBlock::setExp(exptype exp) { this->exp = exp; }
void StatBlock::setMaxHealth(welltype maxHealth) { this->maxHealth = maxHealth; }
void StatBlock::setMaxMana(welltype maxMana) { this->maxMana = maxMana; }

// New setter implementations
void StatBlock::setMovementSpeed(float speed) { this->movementSpeed = speed; }
void StatBlock::setAttackSpeed(float speed) { this->attackSpeed = speed; }
void StatBlock::setDamageMultiplier(float multiplier) { this->damageMultiplier = multiplier; }

// Utility methods
void StatBlock::addExp(exptype amount) {
    exp += amount;
}

bool StatBlock::canLevelUp() const {
    // Simple level up check - you can customize this formula
    return exp >= level * 100;
}

void StatBlock::heal(welltype amount) {
    health += amount;
    if (health > maxHealth) health = maxHealth;
}

void StatBlock::damage(welltype amount) {
    if (amount >= health) {
        health = 0;
    } else {
        health -= amount;
    }
}

void StatBlock::restoreMana(welltype amount) {
    mana += amount;
    if (mana > maxMana) mana = maxMana;
}

void StatBlock::consumeMana(welltype amount) {
    if (amount >= mana) {
        mana = 0;
    } else {
        mana -= amount;
    }
}
// Operator overloads
StatBlock StatBlock::operator+(const StatBlock& other) const {
    return StatBlock(
        strength + other.strength,
        dexterity + other.dexterity,
        intelligence + other.intelligence,
        maxHealth + other.maxHealth,
        maxMana + other.maxMana
    );
}

StatBlock StatBlock::operator-(const StatBlock& other) const {
    return StatBlock(
        strength - other.strength,
        dexterity - other.dexterity,
        intelligence - other.intelligence,
        maxHealth - other.maxHealth,
        maxMana - other.maxMana
    );
}
