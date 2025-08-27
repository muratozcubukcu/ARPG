#include "ability.h"
#include "character.h"
#include "mob.h"
#include "gameengine.h"
#include <cmath>
#include <string>
#include <iostream>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Constructor implementation
Ability::Ability(std::string name, std::string description, AbilityType type, welltype amount, welltype manaCost, 
                 welltype cooldown, welltype castTime, welltype range, AbilityTarget target, 
                 AbilityEffect effect, AbilityActivation activation, AbilityCastType castType, 
                 AbilityShape shape, float projectileSpeed, float effectRadius)
    : name(name), description(description), type(type), manaCost(manaCost), cooldown(cooldown), 
      castTime(castTime), range(range), amount(amount), target(target), 
      effect(effect), activation(activation), castType(castType), shape(shape), 
      projectileSpeed(projectileSpeed), effectRadius(effectRadius) {
}

// Getter implementations
std::string Ability::getName() const { return name; }
std::string Ability::getDescription() const { return description; }
AbilityType Ability::getType() const { return type; }
welltype Ability::getManaCost() const { return manaCost; }
welltype Ability::getCooldown() const { return cooldown; }
welltype Ability::getCastTime() const { return castTime; }
welltype Ability::getRange() const { return range; }
welltype Ability::getAmount() const { return amount; }
AbilityTarget Ability::getTarget() const { return target; }
AbilityEffect Ability::getEffect() const { return effect; }
AbilityActivation Ability::getActivation() const { return activation; }

// Setter implementations
void Ability::setName(std::string name) { this->name = name; }
void Ability::setDescription(std::string description) { this->description = description; }
void Ability::setType(AbilityType type) { this->type = type; }
void Ability::setManaCost(welltype manaCost) { this->manaCost = manaCost; }
void Ability::setCooldown(welltype cooldown) { this->cooldown = cooldown; }
void Ability::setCastTime(welltype castTime) { this->castTime = castTime; }
void Ability::setRange(welltype range) { this->range = range; }
void Ability::setAmount(welltype amount) { this->amount = amount; }
void Ability::setTarget(AbilityTarget target) { this->target = target; }
void Ability::setEffect(AbilityEffect effect) { this->effect = effect; }
void Ability::setActivation(AbilityActivation activation) { this->activation = activation; }

// Utility methods
bool Ability::isPhysical() const { return type == PHYSICAL; }
bool Ability::isMagical() const { return type == MAGICAL; }
bool Ability::isHealing() const { return type == HEALING; }
bool Ability::isUtility() const { return type == UTILITY; }

// Damage calculation methods
welltype Ability::calculateDamage(stattype strength, stattype intelligence) const {
    if (effect != DAMAGE) return 0;
    
    welltype baseDamage = amount;
    welltype statBonus = 0;
    
    if (type == PHYSICAL) {
        // Physical abilities scale with strength: +0.2 per point, rounded
        double bonus = strength * 0.2;
        statBonus = static_cast<welltype>(std::round(bonus));
    } else if (type == MAGICAL) {
        // Magical abilities scale with intelligence: +0.2 per point, rounded
        double bonus = intelligence * 0.2;
        statBonus = static_cast<welltype>(std::round(bonus));
    }
    
    return baseDamage + statBonus;
}

welltype Ability::calculateHeal(stattype intelligence) const {
    if (effect != HEAL) return 0;
    
    welltype baseHeal = amount;
    // Healing abilities scale with intelligence: +0.2 per point, rounded
    double bonus = intelligence * 0.2;
    welltype statBonus = static_cast<welltype>(std::round(bonus));
    
    return baseHeal + statBonus;
}

welltype Ability::calculateBuff(stattype strength, stattype intelligence) const {
    if (effect != BUFF) return 0;
    
    welltype baseBuff = amount;
    welltype statBonus = 0;
    
    if (type == PHYSICAL) {
        // Physical buffs scale with strength: +0.3 per point, rounded
        double bonus = strength * 0.3;
        statBonus = static_cast<welltype>(std::round(bonus));
    } else if (type == MAGICAL) {
        // Magical buffs scale with intelligence: +0.3 per point, rounded
        double bonus = intelligence * 0.3;
        statBonus = static_cast<welltype>(std::round(bonus));
    } else if (type == HEALING) {
        // Healing-based buffs scale with intelligence: +0.25 per point, rounded
        double bonus = intelligence * 0.25;
        statBonus = static_cast<welltype>(std::round(bonus));
    } else if (type == UTILITY) {
        // Utility buffs scale with both stats: +0.15 per point each, rounded
        double bonus = (strength + intelligence) * 0.15;
        statBonus = static_cast<welltype>(std::round(bonus));
    }
    
    return baseBuff + statBonus;
}

welltype Ability::calculateDebuff(stattype strength, stattype intelligence) const {
    if (effect != DEBUFF) return 0;
    
    welltype baseDebuff = amount;
    welltype statBonus = 0;
    
    if (type == PHYSICAL) {
        // Physical debuffs scale with strength: +0.25 per point, rounded
        double bonus = strength * 0.25;
        statBonus = static_cast<welltype>(std::round(bonus));
    } else if (type == MAGICAL) {
        // Magical debuffs scale with intelligence: +0.3 per point, rounded
        double bonus = intelligence * 0.3;
        statBonus = static_cast<welltype>(std::round(bonus));
    } else if (type == UTILITY) {
        // Utility debuffs scale with intelligence: +0.2 per point, rounded
        double bonus = intelligence * 0.2;
        statBonus = static_cast<welltype>(std::round(bonus));
    }
    
    return baseDebuff + statBonus;
}

// Position-based utility methods
bool Ability::isInRange(const Position& casterPos, const Position& targetPos) const {
    double distance = casterPos.distanceTo(targetPos);
    return distance <= static_cast<double>(range);
}

bool Ability::isInRange(double distance) const {
    return distance <= static_cast<double>(range);
}

double Ability::getRangeAsDouble() const {
    return static_cast<double>(range);
}

// New getter implementations
AbilityCastType Ability::getCastType() const { return castType; }
AbilityShape Ability::getShape() const { return shape; }
float Ability::getProjectileSpeed() const { return projectileSpeed; }
float Ability::getEffectRadius() const { return effectRadius; }

// New casting methods
void Ability::castSelf(Character& caster) const {
    if (caster.getStats().getMana() < manaCost) {
        std::cout << "Not enough mana!" << std::endl;
        return;
    }
    
    caster.consumeMana(manaCost);
    
    if (effect == HEAL) {
        welltype heal = calculateHeal(caster.getStats().getIntelligence());
        caster.heal(heal);
        std::cout << caster.getName() << " casts " << name << " on self for " << heal << " healing!" << std::endl;
    } else if (effect == BUFF) {
        welltype buff = calculateBuff(caster.getStats().getStrength(), caster.getStats().getIntelligence());
        applyBuff(caster, buff);
        std::cout << caster.getName() << " casts " << name << " on self for " << buff << " buff!" << std::endl;
    }
}

void Ability::castProjectile(Character& caster, const Position& direction, ProjectileManager& projectileManager) const {
    if (caster.getStats().getMana() < manaCost) {
        std::cout << "Not enough mana!" << std::endl;
        return;
    }
    
    // Check if this ability can actually be cast as a projectile
    if (castType != PROJECTILE_CAST) {
        std::cout << "This ability cannot be cast as a projectile!" << std::endl;
        return;
    }
    
    // Check if projectile speed is set
    if (projectileSpeed <= 0.0f) {
        std::cout << "Warning: Projectile speed not set for " << name << ", using default speed of 10.0" << std::endl;
        const_cast<Ability*>(this)->projectileSpeed = 10.0f; // Set a default speed
    }
    
    caster.consumeMana(manaCost);
    
    std::cout << caster.getName() << " casts " << name << " projectile in direction " << direction << "!" << std::endl;
    
    // Spawn projectile using the ProjectileManager
    projectileManager.spawnProjectile(*this, caster, direction);
}

// Legacy compatibility method - falls back to instant simulation
void Ability::castProjectile(Character& caster, const Position& direction, std::vector<Character>& characters, std::vector<Mob>& mobs) const {
    if (caster.getStats().getMana() < manaCost) {
        std::cout << "Not enough mana!" << std::endl;
        return;
    }
    
    caster.consumeMana(manaCost);
    
    std::cout << caster.getName() << " casts " << name << " projectile (legacy mode) in direction " << direction << "!" << std::endl;
    
    // Fall back to the legacy instant simulation method
    simulateProjectilePath(caster, direction, characters, mobs);
}

void Ability::castGroundTarget(Character& caster, const Position& targetPos, std::vector<Character>& characters, std::vector<Mob>& mobs) const {
    // Check if target position is within range
    if (caster.getPosition().distanceTo(targetPos) > range) {
        std::cout << "Target out of range!" << std::endl;
        return;
    }
    
    if (caster.getStats().getMana() < manaCost) {
        std::cout << "Not enough mana!" << std::endl;
        return;
    }
    
    caster.consumeMana(manaCost);
    
    std::cout << caster.getName() << " casts " << name << " at " << targetPos << "!" << std::endl;
    
    // Get targets in area of effect
    auto charTargets = getTargetsInArea(targetPos, characters);
    auto mobTargets = getTargetsInArea(targetPos, mobs);
    
    if (effect == DAMAGE) {
        welltype damage = calculateDamage(caster.getStats().getStrength(), caster.getStats().getIntelligence());
        
        for (auto& character : charTargets) {
            if (character.getName() != caster.getName()) {
                character.damage(damage);
                std::cout << name << " hits " << character.getName() << " for " << damage << " damage!" << std::endl;
            }
        }
        
        for (auto& mob : mobTargets) {
            mob.damage(damage);
            std::cout << name << " hits " << mob.getDescription() << " for " << damage << " damage!" << std::endl;
        }
    } else if (effect == HEAL) {
        welltype heal = calculateHeal(caster.getStats().getIntelligence());
        
        for (auto& character : charTargets) {
            if (character.getName() != caster.getName()) {
                character.heal(heal);
                std::cout << name << " heals " << character.getName() << " for " << heal << " healing!" << std::endl;
            }
        }
        
        for (auto& mob : mobTargets) {
            mob.heal(heal);
            std::cout << name << " heals " << mob.getDescription() << " for " << heal << " healing!" << std::endl;
        }
    } else if (effect == BUFF) {
        welltype buff = calculateBuff(caster.getStats().getStrength(), caster.getStats().getIntelligence());
        
        for (auto& character : charTargets) {
            if (character.getName() != caster.getName()) {
                applyBuff(character, buff);
                std::cout << name << " buffs " << character.getName() << " for " << buff << " buff!" << std::endl;
            }
        }
        
        for (auto& mob : mobTargets) {
            applyBuff(mob, buff);
            std::cout << name << " buffs " << mob.getDescription() << " for " << buff << " buff!" << std::endl;
        }
    } else if (effect == DEBUFF) {
        welltype debuff = calculateDebuff(caster.getStats().getStrength(), caster.getStats().getIntelligence());
        
        for (auto& character : charTargets) {
            if (character.getName() != caster.getName()) {
                applyDebuff(character, debuff);
                std::cout << name << " debuffs " << character.getName() << " for " << debuff << " debuff!" << std::endl;
            }
        }
        
        for (auto& mob : mobTargets) {
            applyDebuff(mob, debuff);
            std::cout << name << " debuffs " << mob.getDescription() << " for " << debuff << " debuff!" << std::endl;
        }
    }
}

// Hit detection methods
bool Ability::checkProjectileHit(const Position& start, const Position& end, const Position& target, double targetRadius) const {
    // Calculate closest point on projectile line to target
    Position projectileDir = (end - start).normalize();
    Position toTarget = target - start;
    
    double projection = toTarget.dot(projectileDir);
    if (projection < 0 || projection > (end - start).length()) {
        return false; // Target not in projectile path
    }
    
    Position closestPoint = start + projectileDir * projection;
    double distance = (target - closestPoint).length();
    
    return distance <= targetRadius;
}

// Projectile simulation with hitbox collision
void Ability::simulateProjectilePath(Character& caster, const Position& direction, std::vector<Character>& characters, std::vector<Mob>& mobs) const {
    Position start = caster.getPosition();
    Position end = start + direction * range;
    
    std::cout << "Simulating projectile path from " << start << " to " << end << std::endl;
    
    // Check for hits along the projectile path
    bool hit = false;
    
    // Check characters
    // for (auto& character : characters) {
    //     if (character.getName() != caster.getName() && checkProjectileHit(start, end, character.getPosition(), 1.0)) {
    //         welltype damage = calculateDamage(caster.getStats().getStrength(), caster.getStats().getIntelligence());
    //         character.damage(damage);
    //         std::cout << "Projectile hits " << character.getName() << " for " << damage << " damage!" << std::endl;
    //         hit = true;
    //         // Don't break - projectile can hit multiple targets!
    //     }
    // }
    
    // Check mobs
    for (auto& mob : mobs) {
        if (checkProjectileHit(start, end, mob.getPosition(), 1.0)) {
            if (effect == DAMAGE) {
                welltype damage = calculateDamage(caster.getStats().getStrength(), caster.getStats().getIntelligence());
                mob.damage(damage);
                std::cout << "Projectile hits " << mob.getDescription() << " for " << damage << " damage!" << std::endl;
            } else if (effect == HEAL) {
                welltype heal = calculateHeal(caster.getStats().getIntelligence());
                mob.heal(heal);
                std::cout << "Projectile heals " << mob.getDescription() << " for " << heal << " healing!" << std::endl;
            } else if (effect == BUFF) {
                welltype buff = calculateBuff(caster.getStats().getStrength(), caster.getStats().getIntelligence());
                applyBuff(mob, buff);
                std::cout << "Projectile buffs " << mob.getDescription() << " for " << buff << " buff!" << std::endl;
            } else if (effect == DEBUFF) {
                welltype debuff = calculateDebuff(caster.getStats().getStrength(), caster.getStats().getIntelligence());
                applyDebuff(mob, debuff);
                std::cout << "Projectile debuffs " << mob.getDescription() << " for " << debuff << " debuff!" << std::endl;
            }
            hit = true;
            // Don't break - projectile can hit multiple targets!
        }
    }
    
    if (!hit) {
        std::cout << "Projectile missed all targets!" << std::endl;
    } else {
        std::cout << "Projectile hit " << (hit ? "targets" : "nothing") << "!" << std::endl;
    }
}

std::vector<Character> Ability::getTargetsInArea(const Position& center, const std::vector<Character>& characters) const {
    std::vector<Character> targets;
    
    for (const auto& character : characters) {
        if (isTargetInShape(center, character.getPosition())) {
            targets.push_back(character);
        }
    }
    return targets;
}

std::vector<Mob> Ability::getTargetsInArea(const Position& center, const std::vector<Mob>& mobs) const {
    std::vector<Mob> targets;
    
    for (const auto& mob : mobs) {
        if (isTargetInShape(center, mob.getPosition())) {
            targets.push_back(mob);
        }
    }
    return targets;
}

// Shape-based target detection methods
bool Ability::isTargetInShape(const Position& origin, const Position& target) const {
    switch (shape) {
        case SINGLE_TARGET:
            return origin.distanceTo(target) <= effectRadius;
            
        case CIRCLE:
            return isTargetInCircle(origin, target, effectRadius);
            
        case SPHERE:
            return isTargetInSphere(origin, target, effectRadius);
            
        case CONE: {
            // For cone abilities, we need a direction. Use the vector from origin to target as direction
            Position direction = (target - origin).normalize();
            float coneAngle = 45.0f; // Default 45-degree cone
            return isTargetInCone(origin, target, direction, coneAngle);
        }
        
        case LINE: {
            // For line abilities, use the vector from origin to target as direction
            Position direction = (target - origin).normalize();
            float lineWidth = effectRadius; // Use effectRadius as line width
            return isTargetInLine(origin, target, direction, lineWidth);
        }
        
        default:
            // Fallback to circle
            return isTargetInCircle(origin, target, effectRadius);
    }
}

bool Ability::isTargetInCone(const Position& origin, const Position& target, const Position& direction, float angle) const {
    // Check if target is within range first
    double distance = origin.distanceTo(target);
    if (distance > effectRadius) {
        return false;
    }
    
    // Calculate the angle between the direction vector and the vector to the target
    Position toTarget = (target - origin).normalize();
    double dotProduct = direction.dot(toTarget);
    
    // Convert angle from degrees to radians and get the cosine of half the cone angle
    double halfAngleRad = (angle / 2.0) * M_PI / 180.0;
    double cosHalfAngle = std::cos(halfAngleRad);
    
    // If the dot product is greater than the cosine of half the cone angle, target is in cone
    return dotProduct >= cosHalfAngle;
}

bool Ability::isTargetInLine(const Position& origin, const Position& target, const Position& direction, float width) const {
    // Check if target is within range first
    double distance = origin.distanceTo(target);
    if (distance > effectRadius) {
        return false;
    }
    
    // Calculate the closest point on the line to the target
    Position toTarget = target - origin;
    double projection = toTarget.dot(direction);
    
    // If projection is negative, target is behind the origin
    if (projection < 0) {
        return false;
    }
    
    // If projection is beyond the range, target is too far
    if (projection > effectRadius) {
        return false;
    }
    
    // Calculate the perpendicular distance from target to the line
    Position closestPointOnLine = origin + direction * projection;
    double perpDistance = target.distanceTo(closestPointOnLine);
    
    // Check if target is within the line width
    return perpDistance <= width / 2.0;
}

bool Ability::isTargetInCircle(const Position& center, const Position& target, float radius) const {
    // Simple 2D circle check (ignoring Z coordinate for top-down view)
    double distance = center.distance2D(target);
    return distance <= radius;
}

bool Ability::isTargetInSphere(const Position& center, const Position& target, float radius) const {
    // 3D sphere check
    double distance = center.distanceTo(target);
    return distance <= radius;
}

void Ability::cast(Character& caster, Character& target) const {
    if (!isInRange(caster.getPosition(), target.getPosition())) {
        std::cout << "Target out of range!" << std::endl;
        return;
    }
    
    if (caster.getStats().getMana() < manaCost) {
        std::cout << "Not enough mana!" << std::endl;
        return;
    }
    
    // Consume mana
    caster.consumeMana(manaCost);
    
    // Apply effect based on ability type
    if (effect == DAMAGE) {
        welltype damage = calculateDamage(caster.getStats().getStrength(), caster.getStats().getIntelligence());
        target.damage(damage);
        std::cout << caster.getName() << " casts " << name << " on " << target.getName() << " for " << damage << " damage!" << std::endl;
    } else if (effect == HEAL) {
        welltype heal = calculateHeal(caster.getStats().getIntelligence());
        target.heal(heal);
        std::cout << caster.getName() << " casts " << name << " on " << target.getName() << " for " << heal << " healing!" << std::endl;
    } else if (effect == BUFF) {
        welltype buff = calculateBuff(caster.getStats().getStrength(), caster.getStats().getIntelligence());
        applyBuff(target, buff);
        std::cout << caster.getName() << " casts " << name << " on " << target.getName() << " for " << buff << " buff!" << std::endl;
    } else if (effect == DEBUFF) {
        welltype debuff = calculateDebuff(caster.getStats().getStrength(), caster.getStats().getIntelligence());
        applyDebuff(target, debuff);
        std::cout << caster.getName() << " casts " << name << " on " << target.getName() << " for " << debuff << " debuff!" << std::endl;
    }
}

void Ability::cast(Character& caster, Mob& target) const {
    if (!isInRange(caster.getPosition(), target.getPosition())) {
        std::cout << "Target out of range!" << std::endl;
        return;
    }
    
    if (caster.getStats().getMana() < manaCost) {
        std::cout << "Not enough mana!" << std::endl;
        return;
    }
    
    // Consume mana
    caster.consumeMana(manaCost);
    
    // Apply effect based on ability type
    if (effect == DAMAGE) {
        welltype damage = calculateDamage(caster.getStats().getStrength(), caster.getStats().getIntelligence());
        target.damage(damage);
        std::cout << caster.getName() << " casts " << name << " on " << target.getDescription() << " for " << damage << " damage!" << std::endl;
    } else if (effect == HEAL) {
        welltype heal = calculateHeal(caster.getStats().getIntelligence());
        target.heal(heal);
        std::cout << caster.getName() << " casts " << name << " on " << target.getDescription() << " for " << heal << " healing!" << std::endl;
    } else if (effect == BUFF) {
        welltype buff = calculateBuff(caster.getStats().getStrength(), caster.getStats().getIntelligence());
        applyBuff(target, buff);
        std::cout << caster.getName() << " casts " << name << " on " << target.getDescription() << " for " << buff << " buff!" << std::endl;
    } else if (effect == DEBUFF) {
        welltype debuff = calculateDebuff(caster.getStats().getStrength(), caster.getStats().getIntelligence());
        applyDebuff(target, debuff);
        std::cout << caster.getName() << " casts " << name << " on " << target.getDescription() << " for " << debuff << " debuff!" << std::endl;
    }
}

// Buff and debuff application methods
void Ability::applyBuff(Character& target, welltype buffAmount) const {
    if (shouldBuffStrength()) {
        target.getStats().setStrength(target.getStats().getStrength() + buffAmount);
        std::cout << target.getName() << " gains " << buffAmount << " strength!" << std::endl;
    }
    if (shouldBuffDexterity()) {
        target.getStats().setDexterity(target.getStats().getDexterity() + buffAmount);
        std::cout << target.getName() << " gains " << buffAmount << " dexterity!" << std::endl;
    }
    if (shouldBuffIntelligence()) {
        target.getStats().setIntelligence(target.getStats().getIntelligence() + buffAmount);
        std::cout << target.getName() << " gains " << buffAmount << " intelligence!" << std::endl;
    }
    if (shouldBuffMaxHealth()) {
        target.getStats().setMaxHealth(target.getStats().getMaxHealth() + buffAmount);
        target.getStats().setHealth(target.getStats().getMaxHealth()); // Restore to full
        std::cout << target.getName() << " gains " << buffAmount << " max health!" << std::endl;
    }
    if (shouldBuffMaxMana()) {
        target.getStats().setMaxMana(target.getStats().getMaxMana() + buffAmount);
        target.getStats().setMana(target.getStats().getMaxMana()); // Restore to full
        std::cout << target.getName() << " gains " << buffAmount << " max mana!" << std::endl;
    }
}

void Ability::applyBuff(Mob& target, welltype buffAmount) const {
    if (shouldBuffStrength()) {
        target.getStats().setStrength(target.getStats().getStrength() + buffAmount);
        std::cout << target.getDescription() << " gains " << buffAmount << " strength!" << std::endl;
    }
    if (shouldBuffDexterity()) {
        target.getStats().setDexterity(target.getStats().getDexterity() + buffAmount);
        std::cout << target.getDescription() << " gains " << buffAmount << " dexterity!" << std::endl;
    }
    if (shouldBuffIntelligence()) {
        target.getStats().setIntelligence(target.getStats().getIntelligence() + buffAmount);
        std::cout << target.getDescription() << " gains " << buffAmount << " intelligence!" << std::endl;
    }
    if (shouldBuffMaxHealth()) {
        target.getStats().setMaxHealth(target.getStats().getMaxHealth() + buffAmount);
        target.getStats().setHealth(target.getStats().getMaxHealth()); // Restore to full
        std::cout << target.getDescription() << " gains " << buffAmount << " max health!" << std::endl;
    }
    if (shouldBuffMaxMana()) {
        target.getStats().setMaxMana(target.getStats().getMaxMana() + buffAmount);
        target.getStats().setMana(target.getStats().getMaxMana()); // Restore to full
        std::cout << target.getDescription() << " gains " << buffAmount << " max mana!" << std::endl;
    }
}

void Ability::applyDebuff(Character& target, welltype debuffAmount) const {
    if (shouldBuffStrength()) {
        stattype newStrength = target.getStats().getStrength() - debuffAmount;
        if (newStrength < 1) newStrength = 1; // Minimum strength of 1
        target.getStats().setStrength(newStrength);
        std::cout << target.getName() << " loses " << debuffAmount << " strength!" << std::endl;
    }
    if (shouldBuffDexterity()) {
        stattype newDexterity = target.getStats().getDexterity() - debuffAmount;
        if (newDexterity < 1) newDexterity = 1; // Minimum dexterity of 1
        target.getStats().setDexterity(newDexterity);
        std::cout << target.getName() << " loses " << debuffAmount << " dexterity!" << std::endl;
    }
    if (shouldBuffIntelligence()) {
        stattype newIntelligence = target.getStats().getIntelligence() - debuffAmount;
        if (newIntelligence < 1) newIntelligence = 1; // Minimum intelligence of 1
        target.getStats().setIntelligence(newIntelligence);
        std::cout << target.getName() << " loses " << debuffAmount << " intelligence!" << std::endl;
    }
    if (shouldBuffMaxHealth()) {
        welltype newMaxHealth = target.getStats().getMaxHealth() - debuffAmount;
        if (newMaxHealth < 10) newMaxHealth = 10; // Minimum max health of 10
        target.getStats().setMaxHealth(newMaxHealth);
        if (target.getStats().getHealth() > newMaxHealth) {
            target.getStats().setHealth(newMaxHealth); // Reduce current health if it exceeds new max
        }
        std::cout << target.getName() << " loses " << debuffAmount << " max health!" << std::endl;
    }
    if (shouldBuffMaxMana()) {
        welltype newMaxMana = target.getStats().getMaxMana() - debuffAmount;
        if (newMaxMana < 5) newMaxMana = 5; // Minimum max mana of 5
        target.getStats().setMaxMana(newMaxMana);
        if (target.getStats().getMana() > newMaxMana) {
            target.getStats().setMana(newMaxMana); // Reduce current mana if it exceeds new max
        }
        std::cout << target.getName() << " loses " << debuffAmount << " max mana!" << std::endl;
    }
}

void Ability::applyDebuff(Mob& target, welltype debuffAmount) const {
    if (shouldBuffStrength()) {
        stattype newStrength = target.getStats().getStrength() - debuffAmount;
        if (newStrength < 1) newStrength = 1; // Minimum strength of 1
        target.getStats().setStrength(newStrength);
        std::cout << target.getDescription() << " loses " << debuffAmount << " strength!" << std::endl;
    }
    if (shouldBuffDexterity()) {
        stattype newDexterity = target.getStats().getDexterity() - debuffAmount;
        if (newDexterity < 1) newDexterity = 1; // Minimum dexterity of 1
        target.getStats().setDexterity(newDexterity);
        std::cout << target.getDescription() << " loses " << debuffAmount << " dexterity!" << std::endl;
    }
    if (shouldBuffIntelligence()) {
        stattype newIntelligence = target.getStats().getIntelligence() - debuffAmount;
        if (newIntelligence < 1) newIntelligence = 1; // Minimum intelligence of 1
        target.getStats().setIntelligence(newIntelligence);
        std::cout << target.getDescription() << " loses " << debuffAmount << " intelligence!" << std::endl;
    }
    if (shouldBuffMaxHealth()) {
        welltype newMaxHealth = target.getStats().getMaxHealth() - debuffAmount;
        if (newMaxHealth < 10) newMaxHealth = 10; // Minimum max health of 10
        target.getStats().setMaxHealth(newMaxHealth);
        if (target.getStats().getHealth() > newMaxHealth) {
            target.getStats().setHealth(newMaxHealth); // Reduce current health if it exceeds new max
        }
        std::cout << target.getDescription() << " loses " << debuffAmount << " max health!" << std::endl;
    }
    if (shouldBuffMaxMana()) {
        welltype newMaxMana = target.getStats().getMaxMana() - debuffAmount;
        if (newMaxMana < 5) newMaxMana = 5; // Minimum max mana of 5
        target.getStats().setMaxMana(newMaxMana);
        if (target.getStats().getMana() > newMaxMana) {
            target.getStats().setMana(newMaxMana); // Reduce current mana if it exceeds new max
        }
        std::cout << target.getDescription() << " loses " << debuffAmount << " max mana!" << std::endl;
    }
}

// Helper methods for determining which stats to buff/debuff
bool Ability::shouldBuffStrength() const {
    // Physical abilities typically buff strength
    return type == PHYSICAL || (type == UTILITY && name.find("Strength") != std::string::npos);
}

bool Ability::shouldBuffDexterity() const {
    // Utility abilities and some physical abilities buff dexterity
    return type == UTILITY || (type == PHYSICAL && name.find("Agility") != std::string::npos);
}

bool Ability::shouldBuffIntelligence() const {
    // Magical and healing abilities typically buff intelligence
    return type == MAGICAL || type == HEALING || (type == UTILITY && name.find("Intelligence") != std::string::npos);
}

bool Ability::shouldBuffMaxHealth() const {
    // Healing abilities and defensive buffs typically buff max health
    return type == HEALING || (type == UTILITY && (name.find("Health") != std::string::npos || name.find("Protection") != std::string::npos));
}

bool Ability::shouldBuffMaxMana() const {
    // Magical abilities and mana-related buffs typically buff max mana
    return type == MAGICAL || (type == UTILITY && (name.find("Mana") != std::string::npos || name.find("Magic") != std::string::npos));
}