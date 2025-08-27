#ifndef RACE_H
#define RACE_H

#include "types.h"
#include <string>

class Race {
    private:
        std::string name;
        std::string description;        // Race description
        stattype strengthBonus;         // Racial strength bonus (can be negative)
        stattype dexterityBonus;        // Racial dexterity bonus
        stattype intelligenceBonus;     // Racial intelligence bonus
        welltype healthBonus;           // Racial health bonus
        welltype manaBonus;             // Racial mana bonus
        bool isPlayable;                // Can players choose this race?
        bool isHostile;                 // Is this race naturally hostile?
        
        // Combat modifiers
        welltype attackBonus;           // Racial attack bonus
        welltype defenseBonus;          // Racial defense bonus
        welltype speedBonus;            // Racial speed/movement bonus

    public:
        Race(std::string name, std::string description, stattype strengthBonus, stattype dexterityBonus, 
             stattype intelligenceBonus, welltype healthBonus, welltype manaBonus, bool isPlayable, bool isHostile,
             welltype attackBonus, welltype defenseBonus, welltype speedBonus);
        Race(); // Default constructor

        // Static factory methods for preset races
        static Race createHuman();
        static Race createElf();
        static Race createDwarf();
        static Race createGnome();
        static Race createHalfling();
        static Race createOrc();
        static Race createTroll();
        static Race createGoblin();
        static Race createKobold();
        static Race createLizardman();
        static Race createMinotaur();
        
        // Monster-specific races
        static Race createDragon();
        static Race createUndead();
        static Race createDemon();
        static Race createBeast();
        
        // Getters for individual bonuses
        std::string getName() const;
        std::string getDescription() const;
        stattype getStrengthBonus() const;
        stattype getDexterityBonus() const;
        stattype getIntelligenceBonus() const;
        welltype getHealthBonus() const;
        welltype getManaBonus() const;
        bool getIsPlayable() const;
        bool getIsHostile() const;
        welltype getAttackBonus() const;
        welltype getDefenseBonus() const;
        welltype getSpeedBonus() const;
};

#endif // RACE_H