#pragma once
#ifndef CATA_SRC_WEAKPOINT_H
#define CATA_SRC_WEAKPOINT_H

#include <array>
#include <string>
#include <utility>
#include <vector>

#include "damage.h"
#include "type_id.h"

class Creature;
class JsonArray;
class JsonObject;

// Information about an attack on a weak point.
struct weakpoint_attack {
    // The source of the attack.
    const Creature *source;
    // The target of the attack.
    const Creature *target;
    // The weapon used to make the attack.
    const item *weapon;
    // Weather the attack is a melee attack.
    bool is_melee;
    // Whether the attack a critical hit.
    bool is_crit;
    // The Creature's skill in hitting weak points.
    float wp_skill;

    weakpoint_attack();
};

// An effect that a weakpoint can cause.
struct weakpoint_effect {
    // The type of the effect.
    efftype_id effect;
    // The percent chance of causing the effect.
    float chance;
    // Whether the effect is permanent.
    bool permanent;
    // The range of the durations (in turns) of the effect.
    std::pair<int, int> duration;
    // The range of the intensities of the effect.
    std::pair<int, int> intensity;
    // The range of damage, as a percentage of max health, required to the effect.
    std::pair<float, float> damage_required;
    // The message to print, if the player causes the effect.
    std::string message;

    weakpoint_effect();
    // Maybe apply an effect to the target.
    void apply_to( Creature &target, int total_damage, const weakpoint_attack &attack ) const;
    void load( const JsonObject &jo );
};

struct weakpoint {
    // ID of the weakpoint. Equal to the name, if not provided.
    std::string id;
    // Name of the weakpoint. Can be empty.
    std::string name;
    // Percent chance of hitting the weakpoint. Can be increased by skill.
    float coverage = 100.0f;
    // Multiplier for existing armor values. Defaults to 1.
    std::array<float, static_cast<int>( damage_type::NUM )> armor_mult;
    // Flat penalty to armor values. Applied after the multiplier.
    std::array<float, static_cast<int>( damage_type::NUM )> armor_penalty;
    // Damage multipliers. Applied after armor.
    std::array<float, static_cast<int>( damage_type::NUM )> damage_mult;
    // Critical damage multiplers. Applied after armor instead of damage_mult, if the attack is a crit.
    std::array<float, static_cast<int>( damage_type::NUM )>crit_mult;
    // Difficulty to hit the weak point.
    float difficulty = -10.0f;
    // A list of required effects.
    std::vector<efftype_id> required_effects;
    // A list of effects that may trigger by hitting this weak point.
    std::vector<weakpoint_effect> effects;

    weakpoint();
    // Apply the armor multipliers and offsets to a set of resistances.
    void apply_to( resistances &resistances ) const;
    // Apply the damage multiplers to a set of damage values.
    void apply_to( damage_instance &damage, bool is_crit ) const;
    void apply_effects( Creature &target, int total_damage, const weakpoint_attack &attack ) const;
    // Return the change of the creature hitting the weakpoint.
    float hit_chance( const weakpoint_attack &attack ) const;
    void load( const JsonObject &jo );
};

struct weakpoints {
    // List of weakpoints. Each weakpoint should have a unique id.
    std::vector<weakpoint> weakpoint_list;
    // Default weakpoint to return.
    weakpoint default_weakpoint;

    // Selects a weakpoint to hit.
    const weakpoint *select_weakpoint( const weakpoint_attack &attack ) const;

    void clear();
    void load( const JsonArray &ja );
    void remove( const JsonArray &ja );
};

#endif // CATA_SRC_WEAKPOINT_H
