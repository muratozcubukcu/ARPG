#ifndef ABILITY_H
#define ABILITY_H

#include "types.h"
#include "position.h"
#include <string>
#include <vector>

// Forward declarations
class Character;
class Mob;

enum AbilityType {
    PHYSICAL,
    MAGICAL,
    HEALING,
    UTILITY,
};

enum AbilityTarget {
    SELF,           // Only affects caster
    ENEMY,          // Can target enemies
    PROJECTILE,     // Can be cast anywhere, hits what it collides with
    GROUND_TARGET   // Place at specific location, affects area
};

enum AbilityEffect {
    DAMAGE,
    HEAL,
    BUFF,
    DEBUFF
};

enum AbilityActivation {
    ACTIVE,
    PASSIVE,
};

enum AbilityCastType {
    INSTANT,        // Immediate effect
    PROJECTILE_CAST, // Travels through air
    BEAM,          // Continuous beam
    GROUND_CAST,   // Place at location
    CHANNELED      // Continuous while held
};

enum AbilityShape {
    SINGLE_TARGET, // Single target
    LINE, // Line of targets
    CONE, // Cone of targets
    CIRCLE, // Circle of targets
    SPHERE // Sphere of targets
};

class Ability {
    private:
        std::string name;
        std::string description;
        AbilityType type;
        welltype manaCost;
        welltype cooldown;
        welltype castTime;
        welltype range;
        welltype amount;
        AbilityTarget target;
        AbilityEffect effect;
        AbilityActivation activation;
        AbilityCastType castType;
        AbilityShape shape;
        float projectileSpeed;      // For projectile abilities
        float effectRadius;         // For AoE abilities

    public:
        Ability(std::string name, std::string description, AbilityType type, welltype amount, welltype manaCost, 
                welltype cooldown, welltype castTime, welltype range, AbilityTarget target, 
                AbilityEffect effect, AbilityActivation activation, AbilityCastType castType = INSTANT, 
                AbilityShape shape = SINGLE_TARGET, float projectileSpeed = 0.0f, float effectRadius = 0.0f);
        
        // Getters
        std::string getName() const;
        std::string getDescription() const;
        AbilityType getType() const;
        welltype getManaCost() const;
        welltype getCooldown() const;
        welltype getCastTime() const;
        welltype getRange() const;
        welltype getAmount() const;
        AbilityTarget getTarget() const;
        AbilityEffect getEffect() const;
        AbilityActivation getActivation() const;
        
        // Setters
        void setName(std::string name);
        void setDescription(std::string description);
        void setType(AbilityType type);
        void setManaCost(welltype manaCost);
        void setCooldown(welltype cooldown);
        void setCastTime(welltype castTime);
        void setRange(welltype range);
        void setAmount(welltype amount);
        void setTarget(AbilityTarget target);
        void setEffect(AbilityEffect effect);
        void setActivation(AbilityActivation activation);
        
        // Utility methods
        void setDamage(welltype damage, welltype heal, welltype manaGain);
        bool isPhysical() const;
        bool isMagical() const;
        bool isHealing() const;
        bool isUtility() const;
        
        // Damage calculation methods
        welltype calculateDamage(stattype strength, stattype intelligence) const;
        welltype calculateHeal(stattype intelligence) const;

        // Buff calculation methods
        welltype calculateBuff(stattype strength, stattype intelligence) const;
        welltype calculateDebuff(stattype strength, stattype intelligence) const;

        // Passive calculation methods
        welltype calculatePassive(stattype strength, stattype intelligence) const;
        
        // Position-based utility methods
        bool isInRange(const Position& casterPos, const Position& targetPos) const;
        bool isInRange(double distance) const;
        double getRangeAsDouble() const;
        
        // New casting methods (updated for game engine integration)
        void castSelf(Character& caster) const;
        void castProjectile(Character& caster, const Position& direction, class ProjectileManager& projectileManager) const;
        void castProjectile(Character& caster, const Position& direction, std::vector<Character>& characters, std::vector<Mob>& mobs) const; // Legacy compatibility
        void castGroundTarget(Character& caster, const Position& targetPos, std::vector<Character>& characters, std::vector<Mob>& mobs) const;
        
        // Hit detection methods
        bool checkProjectileHit(const Position& start, const Position& end, const Position& target, double targetRadius = 1.0) const;
        std::vector<Character> getTargetsInArea(const Position& center, const std::vector<Character>& characters) const;
        std::vector<Mob> getTargetsInArea(const Position& center, const std::vector<Mob>& mobs) const;
        
        // Shape-based target detection methods
        bool isTargetInShape(const Position& origin, const Position& target) const;
        bool isTargetInCone(const Position& origin, const Position& target, const Position& direction, float angle) const;
        bool isTargetInLine(const Position& origin, const Position& target, const Position& direction, float width) const;
        bool isTargetInCircle(const Position& center, const Position& target, float radius) const;
        bool isTargetInSphere(const Position& center, const Position& target, float radius) const;
        
        // Legacy projectile simulation (deprecated - use ProjectileManager instead)
        void simulateProjectilePath(Character& caster, const Position& direction, std::vector<Character>& characters, std::vector<Mob>& mobs) const;
        
        // Getters for new properties
        AbilityCastType getCastType() const;
        AbilityShape getShape() const;
        float getProjectileSpeed() const;
        float getEffectRadius() const;

        // Casting methods
        void cast(Character& caster, Character& target) const;
        void cast(Character& caster, Mob& target) const;
        
        // Buff and debuff application methods
        void applyBuff(Character& target, welltype buffAmount) const;
        void applyBuff(Mob& target, welltype buffAmount) const;
        void applyDebuff(Character& target, welltype debuffAmount) const;
        void applyDebuff(Mob& target, welltype debuffAmount) const;
        
        // Helper methods for determining which stats to buff/debuff
        bool shouldBuffStrength() const;
        bool shouldBuffDexterity() const;
        bool shouldBuffIntelligence() const;
        bool shouldBuffMaxHealth() const;
        bool shouldBuffMaxMana() const;
};

#endif // ABILITY_H