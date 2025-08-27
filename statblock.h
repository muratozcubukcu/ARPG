#ifndef STATBLOCK_H
#define STATBLOCK_H

#include "types.h"

class StatBlock {
    private:
        stattype strength;
        stattype dexterity;
        stattype intelligence;
        welltype health;
        welltype mana;
        welltype maxHealth;
        welltype maxMana;
        leveltype level;
        exptype exp;
        
        // New stats for missing StatusEffect types
        float movementSpeed;      // Movement speed multiplier (1.0 = normal)
        float attackSpeed;        // Attack speed multiplier (1.0 = normal)
        float damageMultiplier;   // Damage taken multiplier (1.0 = normal)

    public:
        // Constructors
        StatBlock(stattype strength, stattype dexterity, stattype intelligence, 
                 welltype health, welltype mana, leveltype level, exptype exp);
        StatBlock(stattype strength, stattype dexterity, stattype intelligence, 
                 welltype maxHealth, welltype maxMana);
        StatBlock(); // Default constructor
        
        // Getters
        stattype getStrength() const;
        stattype getDexterity() const;
        stattype getIntelligence() const;
        welltype getHealth() const;
        welltype getMana() const;
        leveltype getLevel() const;
        exptype getExp() const;
        welltype getMaxHealth() const;
        welltype getMaxMana() const;
        
        // New getters for missing stats
        float getMovementSpeed() const;
        float getAttackSpeed() const;
        float getDamageMultiplier() const;

        
        // Setters
        void setStrength(stattype strength);
        void setDexterity(stattype dexterity);
        void setIntelligence(stattype intelligence);
        void setHealth(welltype health);
        void setMana(welltype mana);
        void setLevel(leveltype level);
        void setExp(exptype exp);
        void setMaxHealth(welltype maxHealth);
        void setMaxMana(welltype maxMana);
        
        // New setters for missing stats
        void setMovementSpeed(float speed);
        void setAttackSpeed(float speed);
        void setDamageMultiplier(float multiplier);

        
        // Utility methods
        void addExp(exptype amount);
        bool canLevelUp() const;
        void heal(welltype amount);
        void damage(welltype amount);
        void restoreMana(welltype amount);
        void consumeMana(welltype amount);
        
        // Operator overloads for combining stats
        StatBlock operator+(const StatBlock& other) const;
        StatBlock operator-(const StatBlock& other) const;
};

#endif // STATBLOCK_H
