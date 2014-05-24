#pragma once

#include "Damage.h"

#include <chrono>

class Action;
class Actor;
class Aura;

class Model {
	public:
		virtual ~Model() = default;

		virtual Damage generateDamage(const Action* action, const Actor* actor) const = 0;
		virtual Damage generateAutoAttackDamage(const Actor* actor) const = 0;

		virtual Damage acceptDamage(const Damage& incoming, const Actor* actor) const = 0;

		virtual std::chrono::microseconds globalCooldown(const Actor* actor) const = 0;
		virtual std::chrono::microseconds autoAttackInterval(const Actor* actor) const = 0;
		
		virtual double baseTickDamage(const Actor* source, const Aura* aura) const = 0;
		virtual double tickCriticalHitChance(const Actor* source) const = 0;
};