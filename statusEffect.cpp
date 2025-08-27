#include "statuseffect.h"
#include "character.h"
#include "mob.h"
#include <iostream>
#include <algorithm>


// Constructor
StatusEffect::StatusEffect(std::string name, std::string description, StatusEffectType type, 
                         welltype magnitude, float duration, StatusEffectStackType stackType, float tickInterval)
    : name(name), description(description), type(type), stackType(stackType),
      magnitude(magnitude), duration(duration), remainingTime(duration), stacks(1),
      sourceName(""), isDebuff(false), tickInterval(tickInterval), nextTickTime(0.0f),
      characterTarget(nullptr), mobTarget(nullptr) {
    
    // Determine if this is a debuff based on type
    isDebuff = (type >= DEBUFF_STRENGTH && type <= DEBUFF_MAX_MANA) || 
               (type >= STUN && type <= VULNERABILITY);
    
    // Debug tick interval
    if (tickInterval > 0.0f) {
        std::cout << "DEBUG: StatusEffect created - " << name << " with tickInterval=" << tickInterval << "s" << std::endl;
    }
}

// Core methods
void StatusEffect::apply(Character& target, const std::string& source) {
    characterTarget = &target;
    mobTarget = nullptr;
    sourceName = source;
    
    // Apply the effect immediately
    applyEffect();
    
    std::cout << target.getName() << " gains " << name << " from " << source << " for " << duration << " seconds!" << std::endl;
}

void StatusEffect::apply(Mob& target, const std::string& source) {
    mobTarget = &target;
    characterTarget = nullptr;
    sourceName = source;
    
    // Apply the effect immediately
    applyEffect();
    
    std::cout << target.getDescription() << " gains " << name << " from " << source << " for " << duration << " seconds!" << std::endl;
}

void StatusEffect::update(float deltaTime) {
    if (isExpired()) return;
    
    // Update remaining time
    remainingTime -= deltaTime;
    
    // Handle tick effects
    if (tickInterval > 0.0f) {
        std::cout << "DEBUG: Tick check - " << name << " - BEFORE: nextTickTime=" << nextTickTime << ", deltaTime=" << deltaTime << ", tickInterval=" << tickInterval << std::endl;
        nextTickTime -= deltaTime;
        std::cout << "DEBUG: Tick check - " << name << " - AFTER: nextTickTime=" << nextTickTime << std::endl;
        if (nextTickTime < 0.0f) {
            std::cout << "DEBUG: Tick triggered for " << name << " - Type: " << type << ", Magnitude: " << magnitude << std::endl;
            applyTickEffect();
            nextTickTime = tickInterval;
            std::cout << "DEBUG: Tick reset - " << name << " - nextTickTime set to " << nextTickTime << std::endl;
        }
    }
    
    // Check if expired
    if (isExpired()) {
        remove();
    }
}

void StatusEffect::remove() {
    if (!isExpired()) {
        removeEffect();
        std::cout << (characterTarget ? characterTarget->getName() : mobTarget->getDescription()) 
                  << " loses " << name << "!" << std::endl;
    }
    
    // Clear targets
    characterTarget = nullptr;
    mobTarget = nullptr;
}

bool StatusEffect::isExpired() const {
    return remainingTime <= 0.0f;
}

// Stacking methods
bool StatusEffect::canStackWith(const StatusEffect& other) const {
    if (type != other.type) return false;
    
    switch (stackType) {
        case NONE:
            return false;
        case REFRESH:
            return true; // Can refresh duration
        case STACK_INTENSITY:
        case STACK_DURATION:
            return true; // Can stack
        default:
            return false;
    }
}

void StatusEffect::addStack(const StatusEffect& other) {
    if (!canStackWith(other)) return;
    
    switch (stackType) {
        case REFRESH:
            // Refresh duration and re-apply effect to ensure stats are current
            remainingTime = duration;
            stacks++;
            std::cout << "DEBUG: REFRESH - Duration refreshed to " << remainingTime << "s, Stacks: " << stacks << std::endl;
            
            // Re-apply effect to ensure stats are current
            if (characterTarget) {
                std::cout << "DEBUG: REFRESH - Re-applying effect to character" << std::endl;
                applyEffectToCharacter(*characterTarget);
            } else if (mobTarget) {
                std::cout << "DEBUG: REFRESH - Re-applying effect to mob" << std::endl;
                applyEffectToMob(*mobTarget);
            }
            break;
            
        case STACK_INTENSITY:
            // For intensity stacking, we need to remove the old effect and apply the new one
            if (characterTarget) {
                // Remove old effect (restore original stats)
                removeEffectFromCharacter(*characterTarget);
                // Increase magnitude
                magnitude += other.magnitude;
                stacks++;
                // Apply new effect with updated magnitude
                applyEffectToCharacter(*characterTarget);
            } else if (mobTarget) {
                // Remove old effect (restore original stats)
                removeEffectFromMob(*mobTarget);
                // Increase magnitude
                magnitude += other.magnitude;
                stacks++;
                // Apply new effect with updated magnitude
                applyEffectToMob(*mobTarget);
            }
            std::cout << "DEBUG: STACK_INTENSITY - New magnitude: " << magnitude << ", Stacks: " << stacks << std::endl;
            break;
            
        case STACK_DURATION:
            // Increase duration and re-apply effect to ensure stats are current
            remainingTime += other.duration;
            duration += other.duration;
            stacks++;
            std::cout << "DEBUG: STACK_DURATION - Duration increased to " << remainingTime << "s, Stacks: " << stacks << std::endl;
            
            // Re-apply effect to ensure stats are current
            if (characterTarget) {
                std::cout << "DEBUG: STACK_DURATION - Re-applying effect to character" << std::endl;
                applyEffectToCharacter(*characterTarget);
            } else if (mobTarget) {
                std::cout << "DEBUG: STACK_DURATION - Re-applying effect to mob" << std::endl;
                applyEffectToMob(*mobTarget);
            }
            break;
            
        default:
            break;
    }
    
    std::cout << name << " stacks increased to " << stacks << "!" << std::endl;
}

// Effect application methods
void StatusEffect::applyEffect() {
    if (characterTarget) {
        applyEffectToCharacter(*characterTarget);
    } else if (mobTarget) {
        applyEffectToMob(*mobTarget);
    }
}

void StatusEffect::removeEffect() {
    if (characterTarget) {
        removeEffectFromCharacter(*characterTarget);
    } else if (mobTarget) {
        removeEffectFromMob(*mobTarget);
    }
}

void StatusEffect::applyTickEffect() {
    if (characterTarget) {
        applyTickEffectToCharacter(*characterTarget);
    } else if (mobTarget) {
        applyTickEffectToMob(*mobTarget);
    }
}

// Character-specific effect application
void StatusEffect::applyEffectToCharacter(Character& target) {
    switch (type) {
        case BUFF_STRENGTH:
            target.modifyStrength(magnitude);
            break;
        case BUFF_DEXTERITY:
            target.modifyDexterity(magnitude);
            break;
        case BUFF_INTELLIGENCE:
            target.modifyIntelligence(magnitude);
            break;
        case BUFF_MAX_HEALTH:
            target.modifyMaxHealth(magnitude);
            target.getStats().setHealth(target.getStats().getMaxHealth()); // Restore to full
            break;
        case BUFF_MAX_MANA:
            target.modifyMaxMana(magnitude);
            target.getStats().setMana(target.getStats().getMaxMana()); // Restore to full
            break;
        case BUFF_CURRENT_HEALTH:
            target.heal(magnitude);
            break;
        case BUFF_CURRENT_MANA:
            target.getStats().restoreMana(magnitude);
            break;
        case DEBUFF_STRENGTH:
            {
                stattype currentStrength = target.getStats().getStrength();
                stattype reduction = (magnitude < (currentStrength - 1)) ? magnitude : (currentStrength - 1);
                target.modifyStrength(-reduction);
                std::cout << "DEBUG: DEBUFF_STRENGTH - Old: " << currentStrength << ", Reducing: " << reduction << ", New: " << target.getStats().getStrength() << std::endl;
            }
            break;
        case DEBUFF_DEXTERITY:
            {
                stattype currentDexterity = target.getStats().getDexterity();
                stattype reduction = (magnitude < (currentDexterity - 1)) ? magnitude : (currentDexterity - 1);
                target.modifyDexterity(-reduction);
                std::cout << "DEBUG: DEBUFF_DEXTERITY - Old: " << currentDexterity << ", Reducing: " << reduction << ", New: " << target.getStats().getDexterity() << std::endl;
            }
            break;
        case DEBUFF_INTELLIGENCE:
            {
                stattype currentIntelligence = target.getStats().getIntelligence();
                stattype reduction = (magnitude < (currentIntelligence - 1)) ? magnitude : (currentIntelligence - 1);
                target.modifyIntelligence(-reduction);
                std::cout << "DEBUG: DEBUFF_INTELLIGENCE - Old: " << currentIntelligence << ", Reducing: " << reduction << ", New: " << target.getStats().getIntelligence() << std::endl;
            }
            break;
        case DEBUFF_MAX_HEALTH:
            {
                welltype newMaxHealth = target.getStats().getMaxHealth() - magnitude;
                if (newMaxHealth < 10) newMaxHealth = 10;
                target.getStats().setMaxHealth(newMaxHealth);
                if (target.getStats().getHealth() > newMaxHealth) {
                    target.getStats().setHealth(newMaxHealth);
                }
            }
            break;
        case DEBUFF_MAX_MANA:
            {
                welltype newMaxMana = target.getStats().getMaxMana() - magnitude;
                if (newMaxMana < 5) newMaxMana = 5;
                target.getStats().setMaxMana(newMaxMana);
                if (target.getStats().getMana() > newMaxMana) {
                    target.getStats().setMana(newMaxMana);
                }
            }
            break;
        default:
            break;
    }
}

void StatusEffect::removeEffectFromCharacter(Character& target) {
    switch (type) {
        case BUFF_STRENGTH:
            target.getStats().setStrength(target.getStats().getStrength() - magnitude);
            break;
        case BUFF_DEXTERITY:
            target.getStats().setDexterity(target.getStats().getDexterity() - magnitude);
            break;
        case BUFF_INTELLIGENCE:
            target.getStats().setIntelligence(target.getStats().getIntelligence() - magnitude);
            break;
        case BUFF_MAX_HEALTH:
            target.getStats().setMaxHealth(target.getStats().getMaxHealth() - magnitude);
            break;
        case BUFF_MAX_MANA:
            target.getStats().setMaxMana(target.getStats().getMaxMana() - magnitude);
            break;
        case DEBUFF_STRENGTH:
            target.getStats().setStrength(target.getStats().getStrength() + magnitude);
            break;
        case DEBUFF_DEXTERITY:
            target.getStats().setDexterity(target.getStats().getDexterity() + magnitude);
            break;
        case DEBUFF_INTELLIGENCE:
            target.getStats().setIntelligence(target.getStats().getIntelligence() + magnitude);
            break;
        case DEBUFF_MAX_HEALTH:
            target.getStats().setMaxHealth(target.getStats().getMaxHealth() + magnitude);
            break;
        case DEBUFF_MAX_MANA:
            target.getStats().setMaxMana(target.getStats().getMaxMana() + magnitude);
            break;
        default:
            break;
    }
}

void StatusEffect::applyTickEffectToCharacter(Character& target) {
    switch (type) {
        case DOT_DAMAGE:
            target.damage(magnitude);
            std::cout << target.getName() << " takes " << magnitude << " damage over time from " << name << "!" << std::endl;
            break;
        case HOT_HEALING:
            target.heal(magnitude);
            std::cout << target.getName() << " heals " << magnitude << " over time from " << name << "!" << std::endl;
            break;
        default:
            break;
    }
}

// Mob-specific effect application (similar to character but with getDescription())
void StatusEffect::applyEffectToMob(Mob& target) {
    // Same logic as character but using mob methods
    switch (type) {
        case BUFF_STRENGTH:
            target.modifyStrength(magnitude);
            break;
        case BUFF_DEXTERITY:
            target.modifyDexterity(magnitude);
            break;
        case BUFF_INTELLIGENCE:
            target.modifyIntelligence(magnitude);
            break;
        case BUFF_MAX_HEALTH:
            target.getStats().setMaxHealth(target.getStats().getMaxHealth() + magnitude);
            target.getStats().setHealth(target.getStats().getMaxHealth());
            break;
        case BUFF_MAX_MANA:
            target.getStats().setMaxMana(target.getStats().getMaxMana() + magnitude);
            target.getStats().setMana(target.getStats().getMaxMana());
            break;
        case BUFF_CURRENT_HEALTH:
            target.heal(magnitude);
            break;
        case BUFF_CURRENT_MANA:
            target.getStats().restoreMana(magnitude);
            break;
        case DEBUFF_STRENGTH:
            {
                stattype currentStrength = target.getStats().getStrength();
                // For uint16, we need to ensure we don't go below 1
                if (currentStrength > magnitude) {
                    // Calculate final strength value
                    stattype finalStrength = currentStrength - magnitude;
                    target.getStatsRef().setStrength(finalStrength);
                } else {
                    // Set to minimum of 1
                    target.getStatsRef().setStrength(1);
                }
                std::cout << "DEBUG: MOB DEBUFF_STRENGTH - Old: " << currentStrength << ", Reducing by: " << magnitude << ", New: " << target.getStats().getStrength() << std::endl;
            }
            break;
        case DEBUFF_DEXTERITY:
            {
                stattype currentDexterity = target.getStats().getDexterity();
                // For uint16, we need to ensure we don't go below 1
                if (currentDexterity > magnitude) {
                    // Calculate final dexterity value
                    stattype finalDexterity = currentDexterity - magnitude;
                    target.getStatsRef().setDexterity(finalDexterity);
                } else {
                    // Set to minimum of 1
                    target.getStatsRef().setDexterity(1);
                }
                std::cout << "DEBUG: MOB DEBUFF_DEXTERITY - Old: " << currentDexterity << ", Reducing by: " << magnitude << ", New: " << target.getStats().getDexterity() << std::endl;
            }
            break;
        case DEBUFF_INTELLIGENCE:
            {
                stattype currentIntelligence = target.getStats().getIntelligence();
                // For uint16, we need to ensure we don't go below 1
                if (currentIntelligence > magnitude) {
                    // Calculate final intelligence value
                    stattype finalIntelligence = currentIntelligence - magnitude;
                    target.getStatsRef().setIntelligence(finalIntelligence);
                } else {
                    // Set to minimum of 1
                    target.getStatsRef().setIntelligence(1);
                }
                std::cout << "DEBUG: MOB DEBUFF_INTELLIGENCE - Old: " << currentIntelligence << ", Reducing by: " << magnitude << ", New: " << target.getStats().getIntelligence() << std::endl;
            }
            break;
        case DEBUFF_MAX_HEALTH:
            {
                welltype newMaxHealth = target.getStats().getMaxHealth() - magnitude;
                if (newMaxHealth < 10) newMaxHealth = 10;
                target.getStats().setMaxHealth(newMaxHealth);
                if (target.getStats().getHealth() > newMaxHealth) {
                    target.getStats().setHealth(newMaxHealth);
                }
            }
            break;
        case DEBUFF_MAX_MANA:
            {
                welltype newMaxMana = target.getStats().getMaxMana() - magnitude;
                if (newMaxMana < 5) newMaxMana = 5;
                target.getStats().setMaxMana(newMaxMana);
                if (target.getStats().getMana() > newMaxMana) {
                    target.getStats().setMana(newMaxMana);
                }
            }
            break;
            
        // Crowd control effects
        case STUN:
            target.setStunned(true);
            std::cout << target.getDescription() << " is stunned and cannot act!" << std::endl;
            break;
        case SILENCE:
            target.setSilenced(true);
            std::cout << target.getDescription() << " is silenced and cannot cast spells!" << std::endl;
            break;
        case ROOT:
            target.setRooted(true);
            std::cout << target.getDescription() << " is rooted and cannot move!" << std::endl;
            break;
            
        // Speed modification effects
        case SLOW_MOVEMENT:
            {
                float currentSpeed = target.getStats().getMovementSpeed();
                float newSpeed = currentSpeed * (1.0f - (magnitude / 100.0f)); // magnitude as percentage
                if (newSpeed < 0.1f) newSpeed = 0.1f; // Minimum 10% speed
                target.getStats().setMovementSpeed(newSpeed);
                std::cout << target.getDescription() << "'s movement speed reduced to " << (newSpeed * 100) << "%!" << std::endl;
            }
            break;
        case SLOW_ATTACK:
            {
                float currentSpeed = target.getStats().getAttackSpeed();
                float newSpeed = currentSpeed * (1.0f - (magnitude / 100.0f)); // magnitude as percentage
                if (newSpeed < 0.1f) newSpeed = 0.1f; // Minimum 10% speed
                target.getStats().setAttackSpeed(newSpeed);
                std::cout << target.getDescription() << "'s attack speed reduced to " << (newSpeed * 100) << "%!" << std::endl;
            }
            break;
            
        // Damage modification effects
        case VULNERABILITY:
            {
                float currentMultiplier = target.getStats().getDamageMultiplier();
                float newMultiplier = currentMultiplier * (1.0f + (magnitude / 100.0f)); // magnitude as percentage
                target.getStats().setDamageMultiplier(newMultiplier);
                std::cout << target.getDescription() << " takes " << (newMultiplier * 100) << "% damage (vulnerable)!" << std::endl;
            }
            break;
        case RESISTANCE:
            {
                float currentMultiplier = target.getStats().getDamageMultiplier();
                float newMultiplier = currentMultiplier * (1.0f - (magnitude / 100.0f)); // magnitude as percentage
                if (newMultiplier < 0.1f) newMultiplier = 0.1f; // Minimum 10% damage
                target.getStats().setDamageMultiplier(newMultiplier);
                std::cout << target.getDescription() << " takes " << (newMultiplier * 100) << "% damage (resistant)!" << std::endl;
            }
            break;
            
        default:
            break;
    }
}

void StatusEffect::removeEffectFromMob(Mob& target) {
    // Only remove effects for buffs/debuffs, not for DOT/HOT
    if (type == DOT_DAMAGE || type == HOT_HEALING) {
        // DOT/HOT effects should NOT be reverted - they're permanent
        return;
    }
    
    // Same logic as character removal
    switch (type) {
        case BUFF_STRENGTH:
            target.getStatsRef().setStrength(target.getStats().getStrength() - magnitude);
            break;
        case BUFF_DEXTERITY:
            target.getStatsRef().setDexterity(target.getStats().getDexterity() - magnitude);
            break;
        case BUFF_INTELLIGENCE:
            target.getStatsRef().setIntelligence(target.getStats().getIntelligence() - magnitude);
            break;
        case BUFF_MAX_HEALTH:
            target.getStatsRef().setMaxHealth(target.getStats().getMaxHealth() - magnitude);
            break;
        case BUFF_MAX_MANA:
            target.getStatsRef().setMaxMana(target.getStats().getMaxMana() - magnitude);
            break;
        case DEBUFF_STRENGTH:
            target.getStatsRef().setStrength(target.getStats().getStrength() + magnitude);
            break;
        case DEBUFF_DEXTERITY:
            target.getStatsRef().setDexterity(target.getStats().getDexterity() + magnitude);
            break;
        case DEBUFF_INTELLIGENCE:
            target.getStatsRef().setIntelligence(target.getStats().getIntelligence() + magnitude);
            break;
        case DEBUFF_MAX_HEALTH:
            target.getStatsRef().setMaxHealth(target.getStats().getMaxHealth() + magnitude);
            break;
        case DEBUFF_MAX_MANA:
            target.getStatsRef().setMaxMana(target.getStats().getMaxMana() + magnitude);
            break;
            
        // Crowd control effects
        case STUN:
            target.setStunned(false);
            std::cout << target.getDescription() << " is no longer stunned!" << std::endl;
            break;
        case SILENCE:
            target.setSilenced(false);
            std::cout << target.getDescription() << " is no longer silenced!" << std::endl;
            break;
        case ROOT:
            target.setRooted(false);
            std::cout << target.getDescription() << " is no longer rooted!" << std::endl;
            break;
            
        // Speed modification effects
        case SLOW_MOVEMENT:
            target.getStats().setMovementSpeed(1.0f); // Reset to normal
            std::cout << target.getDescription() << "'s movement speed restored to normal!" << std::endl;
            break;
        case SLOW_ATTACK:
            target.getStats().setAttackSpeed(1.0f); // Reset to normal
            std::cout << target.getDescription() << "'s attack speed restored to normal!" << std::endl;
            break;
            
        // Damage modification effects
        case VULNERABILITY:
            target.getStats().setDamageMultiplier(1.0f); // Reset to normal
            std::cout << target.getDescription() << "'s damage vulnerability removed!" << std::endl;
            break;
        case RESISTANCE:
            target.getStats().setDamageMultiplier(1.0f); // Reset to normal
            std::cout << target.getDescription() << "'s damage resistance removed!" << std::endl;
            break;
            
        default:
            break;
    }
}

void StatusEffect::applyTickEffectToMob(Mob& target) {
    switch (type) {
        case DOT_DAMAGE:
            target.damage(magnitude);
            std::cout << target.getDescription() << " takes " << magnitude << " damage over time from " << name << "!" << std::endl;
            break;
        case HOT_HEALING:
            target.heal(magnitude);
            std::cout << target.getDescription() << " heals " << magnitude << " over time from " << name << "!" << std::endl;
            break;
        default:
            break;
    }
}

// Utility methods
std::string StatusEffect::getDisplayName() const {
    if (stacks > 1) {
        return name + " (" + std::to_string(stacks) + ")";
    }
    return name;
}
