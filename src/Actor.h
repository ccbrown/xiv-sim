#pragma once

#include "Damage.h"

#include <chrono>
#include <unordered_map>
#include <string>

using namespace std::literals::chrono_literals;

class Action;
class Aura;
class Model;
class Rotation;

class Actor {
	public:
		struct Stats {
			int weaponDamage = 0;
			double weaponDelay = 0.0;
			int strength = 0;
			int determination = 0;
			int criticalHitRate = 0;
			int skillSpeed = 0;
		};
	
		struct Configuration {
			Stats stats;
			const Model* model = nullptr;
			const Rotation* rotation = nullptr;
		};

		Actor(const Configuration* configuration) : _configuration(configuration), _stats(configuration->stats) {}

		const Action* act(const Actor* target) const;

		const Stats& stats() const { return _stats; }

		Damage generateDamage(const Action* action) const;
		Damage acceptDamage(const Damage& incoming) const;

		void advanceTime(const std::chrono::microseconds& time);
		Damage performAutoAttack();
		void triggerGlobalCooldown();

		std::chrono::microseconds autoAttackDelayRemaining() const;
		std::chrono::microseconds globalCooldownRemaining() const;
		
		void applyAura(Actor* source, Aura* aura);
		void dispelAura(const std::string& identifier);

		int auraCount(const std::string& identifier) const;
		std::chrono::microseconds auraTimeRemaining(const std::string& identifier) const;

		bool isOnGlobalCooldown() const { return globalCooldownRemaining() > 0us; }

		Damage generateTickDamage(const std::string& auraIdentifier) const;
		
		double damageMultiplier() const;
		double autoAttackSpeedMultiplier() const;
		void transformIncomingDamage(Damage* damage) const;
		double additionalCriticalHitChance() const;

		void triggerCooldown(const std::string& identifier, std::chrono::microseconds duration);
		std::chrono::microseconds cooldownRemaining(const std::string& identifier) const;

		int tp() const { return _tp; }
		void setTP(int tp) { _tp = tp; }

		struct AppliedAura {
			Aura* aura = nullptr;
			int count = 0;
			std::chrono::microseconds time = 0us;
			std::chrono::microseconds duration = 0us;
			double baseTickDamage = 0.0;
			double tickCriticalHitChance = 0.0;
		};
		
		const std::unordered_map<std::string, AppliedAura>& auras() const { return _auras; }
		
	private:
		const Configuration* const _configuration = nullptr;
		Stats _stats;

		std::chrono::microseconds _time = 0us;
		std::chrono::microseconds _globalCooldownStartTime = -1min;
		std::chrono::microseconds _lastAutoAttackTime = -1min;
		
		struct Cooldown {
			std::chrono::microseconds time = 0us;
			std::chrono::microseconds duration = 0us;
		};
		
		int _tp = 1000;
		std::unordered_map<std::string, Cooldown> _cooldowns;
		std::unordered_map<std::string, AppliedAura> _auras;
};