#ifndef CHARACTER_H
#define CHARACTER_H

#include "race.h"
#include "class.h"
#include "statblock.h"
#include "position.h"
#include "statuseffect.h"
#include <string>
#include <vector>

// Forward declarations
class Ability;
class Mob;

class Character {
    private:
        std::string name;
        Race race;
        Class characterClass;
        StatBlock finalStats;  // Final calculated stats (Class + Race)
        std::vector<Ability> abilities;  // List of unlocked abilities
        Position position;     // 3D position in the world
        std::vector<StatusEffect> statusEffects;  // Active status effects
        
        // Crowd control flags for missing StatusEffect types
        bool isStunned;        // Cannot act
        bool isSilenced;       // Cannot cast spells
        bool isRooted;         // Cannot move

    public:
        Character(std::string name, Race race, Class characterClass);
        Character(); // Default constructor

        // Getters
        std::string getName() const;
        Race getRace() const;
        Class getCharacterClass() const;
        
        // Get final calculated stats
        StatBlock getStats() const;
        
        // Crowd control getters
        bool getIsStunned() const { return isStunned; }
        bool getIsSilenced() const { return isSilenced; }
        bool getIsRooted() const { return isRooted; }
        
        // Position methods
        Position getPosition() const;
        void setPosition(const Position& pos);
        void setPosition(double x, double y, double z);
        void move(double deltaX, double deltaY, double deltaZ);
        double distanceTo(const Character& other) const;
        double distanceTo(const Position& pos) const;
        
        // Ability methods
        const std::vector<Ability>& getAbilities() const;
        void addAbility(const Ability& ability);
        std::vector<Ability> getNewAbilitiesForLevel(int level) const;
        
        // Status effect management
        void addStatusEffect(const StatusEffect& effect);
        void removeStatusEffect(const std::string& effectName);
        void updateStatusEffects(float deltaTime);
        const std::vector<StatusEffect>& getStatusEffects() const;
        bool hasStatusEffect(const std::string& effectName) const;
        
        // Stat modification methods (for status effects)
        void modifyStrength(stattype amount);
        void modifyDexterity(stattype amount);
        void modifyIntelligence(stattype amount);
        void modifyMaxHealth(welltype amount);
        void modifyMaxMana(welltype amount);
        
        // Crowd control methods (for status effects)
        void setStunned(bool stunned);
        void setSilenced(bool silenced);
        void setRooted(bool rooted);
        
        // Utility methods
        void addExp(exptype amount);
        bool canLevelUp() const;
        void levelUp();
        void heal(welltype amount);
        void damage(welltype amount);
        void restoreMana(welltype amount);
        void consumeMana(welltype amount);
        
        // Character info
        std::string getFullDescription() const;

        // Ability methods
        bool useAbility(const Ability& ability, Character& target);
        bool useAbility(const Ability& ability, Mob& target);
};

#endif // CHARACTER_H