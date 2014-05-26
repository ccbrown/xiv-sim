#pragma once

#include "common.h"
#include "Damage.h"

#include <chrono>
#include <random>
#include <string>
#include <unordered_map>

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
			int intelligence = 0;
			int piety = 0;

			int determination = 0;
			int criticalHitRate = 0;
			int skillSpeed = 0;
			int spellSpeed = 0;
		};

		struct EffectSimulationStats {
			int count = 0;
			int criticalHits = 0;
			int damageDealt = 0;
			
			EffectSimulationStats& operator+=(const EffectSimulationStats& other) {
				count += other.count;
				criticalHits += other.criticalHits;
				damageDealt += other.damageDealt;
				return *this;
			}
		};

		struct SimulationStats {
			int damageDealt = 0;
			std::vector<std::pair<std::chrono::microseconds, int>> tpSamples;
			std::unordered_map<std::string, std::vector<std::pair<std::chrono::microseconds, int>>> auraSamples;

			SimulationStats& operator+=(const SimulationStats& other) {
				damageDealt += other.damageDealt;
				return *this;
			}

			SimulationStats& operator+=(const EffectSimulationStats& effectStats) {
				damageDealt += effectStats.damageDealt;
				return *this;
			}
		};
	
		struct Configuration {
			Stats stats;
			const Model* model = nullptr;
			const Rotation* rotation = nullptr;
			std::random_device* rng = nullptr;
			bool keepsSamples = false;
		};

		Actor(const Configuration* configuration)
			: _configuration(configuration), _stats(configuration->stats), _mp(maximumMP())
		{}

		const Action* act(const Actor* target) const;

		std::random_device& rng() { return *_configuration->rng; }
		const Model* model() const { return _configuration->model; }

		const Stats& stats() const { return _stats; }
		const SimulationStats& simulationStats() const { return _simulationStats; }
		const std::unordered_map<std::string, EffectSimulationStats>& effectSimulationStats() const { return _effectSimulationStats; }

		void integrateDamageStats(const Damage& damage, const char* effect);

		Damage generateDamage(const Action* action);
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

		bool isOnGlobalCooldown() const { return globalCooldownRemaining() > 0_us; }

		Damage generateTickDamage(const std::string& auraIdentifier) const;
		
		double damageMultiplier() const;
		double autoAttackSpeedMultiplier() const;
		void transformIncomingDamage(Damage* damage) const;
		double additionalCriticalHitChance() const;
		
		double globalCooldownMultiplier() const;

		void triggerCooldown(const std::string& identifier, std::chrono::microseconds duration);
		std::chrono::microseconds cooldownRemaining(const std::string& identifier) const;

		int tp() const { return _tp; }
		void setTP(int tp) { _tp = tp; }

		int mp() const { return _mp; }
		void setMP(int mp) { _mp = mp; }
			
		int maximumMP() const;

		struct AppliedAura {
			Aura* aura = nullptr;
			int count = 0;
			std::chrono::microseconds time = 0_us;
			std::chrono::microseconds duration = 0_us;
			double baseTickDamage = 0.0;
			double tickCriticalHitChance = 0.0;
		};
		
		const std::unordered_map<std::string, AppliedAura>& auras() const { return _auras; }
		
	private:
		const Configuration* const _configuration = nullptr;

		Stats _stats;

		std::chrono::microseconds _time = 0_us;
		std::chrono::microseconds _globalCooldownStartTime = -1_min;
		std::chrono::microseconds _lastAutoAttackTime = -1_min;
		
		struct Cooldown {
			std::chrono::microseconds time = 0_us;
			std::chrono::microseconds duration = 0_us;
		};
		
		int _tp = 1000;
		int _mp = 0;

		std::unordered_map<std::string, Cooldown> _cooldowns;
		std::unordered_map<std::string, AppliedAura> _auras;

		SimulationStats _simulationStats;
		std::unordered_map<std::string, EffectSimulationStats> _effectSimulationStats;
		
		void _integrateAuraApplicationCountChange(const char* identifier, int count);
};
