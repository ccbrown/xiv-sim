#pragma once

#include "Damage.h"

#include <chrono>
#include <string>

class Action;
class Actor;
class Aura;

class Model {
	public:
		Model(const char* identifier) : _identifier(identifier) {}
		virtual ~Model() = default;

		const std::string& identifier() const { return _identifier; }

		virtual const Action* action(const char* identifier) const = 0;

		virtual Damage generateDamage(const Action* action, const Actor* source, const Actor* target) const = 0;
		virtual Damage generateAutoAttackDamage(Actor* actor) const = 0;

		virtual Damage acceptDamage(const Damage& incoming, const Actor* actor) const = 0;

		virtual std::chrono::microseconds globalCooldown(const Actor* actor) const = 0;
		virtual std::chrono::microseconds autoAttackInterval(const Actor* actor) const = 0;
		
		virtual double baseTickDamage(const Actor* source, const Aura* aura) const = 0;
		virtual double tickCriticalHitChance(const Actor* source) const = 0;
		virtual std::chrono::microseconds castTime(const Action* action, const Actor* actor) const = 0;

		virtual int maximumMP(const Actor* actor) const = 0;
		
	private:
		std::string _identifier;
};