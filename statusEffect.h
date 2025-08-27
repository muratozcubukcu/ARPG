#ifndef STATUSEFFECT_H
#define STATUSEFFECT_H

#include "types.h"
#include <string>
#include <vector>

// Forward declarations
class Character;
class Mob;

enum StatusEffectType {
    BUFF_STRENGTH,
    BUFF_DEXTERITY,
    BUFF_INTELLIGENCE,
    BUFF_MAX_HEALTH,
    BUFF_MAX_MANA,
    BUFF_CURRENT_HEALTH,
    BUFF_CURRENT_MANA,
    DEBUFF_STRENGTH,
    DEBUFF_DEXTERITY,
    DEBUFF_INTELLIGENCE,
    DEBUFF_MAX_HEALTH,
    DEBUFF_MAX_MANA,
    DOT_DAMAGE,           // Damage over time
    HOT_HEALING,          // Healing over time
    STUN,                 // Cannot act
    SILENCE,              // Cannot cast spells
    ROOT,                 // Cannot move
    SLOW_MOVEMENT,        // Reduced movement speed
    SLOW_ATTACK,          // Reduced attack speed
    VULNERABILITY,        // Takes more damage
    RESISTANCE            // Takes less damage
};

enum StatusEffectStackType {
    NONE,           // No stacking
    REFRESH,        // Refresh duration, don't stack
    STACK_INTENSITY, // Stack effect magnitude
    STACK_DURATION  // Stack duration
};

class StatusEffect {
private:
    std::string name;
    std::string description;
    StatusEffectType type;
    StatusEffectStackType stackType;
    
    // Effect properties
    welltype magnitude;           // How much the effect does
    float duration;               // How long it lasts (seconds)
    float remainingTime;          // Time remaining (seconds)
    int stacks;                   // Current number of stacks
    
    // Source information
    std::string sourceName;       // Who cast this effect
    bool isDebuff;                // Whether this is a negative effect
    
    // Tick properties
    float tickInterval;           // How often to apply tick effects (seconds)
    float nextTickTime;           // When to apply next tick
    
    // Target tracking
    Character* characterTarget;    // Character target (null if mob)
    Mob* mobTarget;               // Mob target (null if character)

public:
    // Constructors
    StatusEffect(std::string name, std::string description, StatusEffectType type, 
                 welltype magnitude, float duration, StatusEffectStackType stackType = NONE,
                 float tickInterval = 0.0f);
    
    // Core methods
    void apply(Character& target, const std::string& source);
    void apply(Mob& target, const std::string& source);
    void update(float deltaTime);
    void remove();
    bool isExpired() const;
    
    // Stacking methods
    bool canStackWith(const StatusEffect& other) const;
    void addStack(const StatusEffect& other);
    
    // Getters
    std::string getName() const { return name; }
    std::string getDescription() const { return description; }
    StatusEffectType getType() const { return type; }
    welltype getMagnitude() const { return magnitude; }
    float getDuration() const { return duration; }
    float getRemainingTime() const { return remainingTime; }
    int getStacks() const { return stacks; }
    std::string getSourceName() const { return sourceName; }
    bool getIsDebuff() const { return isDebuff; }
    float getTickInterval() const { return tickInterval; }
    
    // Utility methods
    std::string getDisplayName() const;
    bool affectsCharacter() const { return characterTarget != nullptr; }
    bool affectsMob() const { return mobTarget != nullptr; }
    
    // Effect application methods
    void applyEffect();
    void removeEffect();
    void applyTickEffect();
    
    // Character-specific effect methods
    void applyEffectToCharacter(Character& target);
    void removeEffectFromCharacter(Character& target);
    void applyTickEffectToCharacter(Character& target);
    
    // Mob-specific effect methods
    void applyEffectToMob(Mob& target);
    void removeEffectFromMob(Mob& target);
    void applyTickEffectToMob(Mob& target);
    
    // Target management
    void setCharacterTarget(Character* target) { characterTarget = target; }
    void setMobTarget(Mob* target) { mobTarget = target; }
    Character* getCharacterTarget() const { return characterTarget; }
    Mob* getMobTarget() const { return mobTarget; }
};

#endif // STATUSEFFECT_H
