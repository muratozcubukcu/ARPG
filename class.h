#ifndef CLASS_H
#define CLASS_H

#include "types.h"
#include <string>
#include <map>
#include <vector>
#include "ability.h"

class Class {
    private:
        std::string name;
        stattype baseStrength;
        stattype baseDexterity;
        stattype baseIntelligence;
        welltype baseMaxHealth;
        welltype baseMaxMana;
        
        // Growth rates per level
        welltype hpGrowth;
        welltype mpGrowth;
        stattype strGrowth;
        stattype dexGrowth;
        stattype intGrowth;
        
        // Level-based abilities
        std::map<int, std::vector<Ability>> levelAbilities;
        
        static const welltype STRGROWTH = 5;
        static const welltype DEXGROWTH = 5;
        static const welltype INTGROWTH = 5;
    public:
        Class(std::string name, stattype strength, stattype dexterity, stattype intelligence, 
              welltype maxHealth, welltype maxMana, welltype hpGrowth, welltype mpGrowth,
              stattype strGrowth, stattype dexGrowth, stattype intGrowth);
        Class(); // Default constructor
        
        // Static factory methods for preset classes
        static Class createWarrior();
        static Class createMage();
        static Class createArcher();
        static Class createPaladin();
        static Class createNone();
        
        // Getters for individual stats
        stattype getBaseStrength() const;
        stattype getBaseDexterity() const;
        stattype getBaseIntelligence() const;
        welltype getBaseMaxHealth() const;
        welltype getBaseMaxMana() const;
        std::string getName() const;
        
        // Growth method
        void applyLevelUpGrowth(class StatBlock& stats) const;
        
        // Ability methods
        void addAbilityForLevel(int level, const Ability& ability);
        std::vector<Ability> getAbilitiesForLevel(int level) const;
        std::vector<Ability> getAllAbilities() const;
};

#endif // CLASS_H



