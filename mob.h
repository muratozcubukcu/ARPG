#include "types.h"
#include "race.h"
#include "statblock.h"
#include "position.h"
#include <string>
#include <vector> // Added for std::vector
#include "statuseffect.h" // Added for StatusEffect

class Mob {
    private:
        Race race;
        StatBlock stats;
        Position position;     // 3D position in the world
        std::vector<StatusEffect> statusEffects;  // Active status effects
        
        // Crowd control flags for missing StatusEffect types
        bool isStunned;        // Cannot act
        bool isSilenced;       // Cannot cast spells
        bool isRooted;         // Cannot move

    public:
        Mob(Race race);
        
        // Basic getter methods
        Race getRace() const;
        StatBlock getStats() const;
        StatBlock& getStatsRef(); // Non-const reference for modifications
        
        // Crowd control getters
        bool getIsStunned() const { return isStunned; }
        bool getIsSilenced() const { return isSilenced; }
        bool getIsRooted() const { return isRooted; }
        
        // Position method implementations
        Position getPosition() const;
        void setPosition(const Position& pos);
        void setPosition(double x, double y, double z);
        void move(double deltaX, double deltaY, double deltaZ);
        double distanceTo(const Mob& other) const;
        double distanceTo(const Position& pos) const;
        
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
        
        // Description methods
        std::string getDescription() const;
        std::string getFullDescription() const;
        
        // Combat methods
        void damage(welltype amount);
        void heal(welltype amount);
        void restoreMana(welltype amount);
        void consumeMana(welltype amount);
};