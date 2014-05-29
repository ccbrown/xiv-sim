#pragma once

#include "common.h"
#include "Aura.h"
#include "Damage.h"

#include <chrono>
#include <random>
#include <string>
#include <utility>
#include <unordered_map>

class Action;
class Model;
class Rotation;

class Actor {
	public:
		struct Stats {
			int weaponDamage = 0;
			double weaponDelay = 0.0;

			int strength = 0;
			int dexterity = 0;
			int intelligence = 0;
			int piety = 0;

			int determination = 0;
			int criticalHitRate = 0;
			int skillSpeed = 0;
			int spellSpeed = 0;
			
			Stats& operator*=(const Aura::StatsMultiplier& multiplier);
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
			std::vector<std::pair<std::chrono::microseconds, int>> mpSamples;
			std::unordered_map<std::string, std::vector<std::pair<std::chrono::microseconds, int>>> auraSamples;
			std::vector<const Action*> actions;

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
			std::string identifier;
			Stats stats;
			const Model* model = nullptr;
			const Rotation* rotation = nullptr;
			bool keepsHistory = false;
			const Configuration* petConfiguration = nullptr;
		};

		Actor(const Configuration* configuration, std::random_device* rng);
		~Actor();

		const std::string& identifier() { return _configuration->identifier; }

		const Action* act(const Actor* target);
		
		void tick();
		
		Actor* pet() { return _pet; }
		const Actor* pet() const { return _pet; }
			
		void setCommand(const Action* action) { _command = action; }
		const Action* command() const { return _command; }

		Actor* owner() { return _owner; }
		const Actor* owner() const { return _owner; }

		std::random_device& rng() const { return *_rng; }
		const Model* model() const { return _configuration->model; }

		const Stats& stats() const { return _stats; }
		const SimulationStats& simulationStats() const { return _simulationStats; }
		const std::unordered_map<std::string, EffectSimulationStats>& effectSimulationStats() const { return _effectSimulationStats; }

		void integrateDamageStats(const Damage& damage, const char* effect);

		Damage generateDamage(const Action* action, const Actor* target);
		Damage acceptDamage(const Damage& incoming) const;

		void advanceTime(const std::chrono::microseconds& time);
		Damage performAutoAttack();
		
		bool executeAction(const Action* action, Actor* target);

		const Action* comboAction() const;

		std::chrono::microseconds autoAttackDelayRemaining() const;
		std::chrono::microseconds globalCooldownRemaining() const;
		std::chrono::microseconds animationLockRemaining() const;
		
		std::chrono::microseconds timeUntilNextTimeOfInterest() const;

		void applyAura(const Aura* aura, Actor* source, int count = 1);
		int dispelAura(const std::string& identifier, Actor* source, int count = 1);
		void extendAura(const std::string& identifier, Actor* source, const std::chrono::microseconds& extension);

		int auraCount(const std::string& identifier, const Actor* source) const;
		std::chrono::microseconds auraTimeRemaining(const std::string& identifier, const Actor* source) const;

		double damageMultiplier() const;
		double autoAttackSpeedMultiplier() const;
		void transformIncomingDamage(Damage* damage) const;
		double additionalCriticalHitChance() const;
		
		double globalCooldownMultiplier() const;

		void triggerCooldown(const std::string& identifier, std::chrono::microseconds duration);
		std::chrono::microseconds cooldownRemaining(const std::string& identifier) const;

		bool beginCast(const Action* action, Actor* target);
		const Action* currentCast(std::chrono::microseconds* remaining = nullptr, Actor** target = nullptr) const;

		void triggerGlobalCooldown();
		void triggerAnimationLock(std::chrono::microseconds duration);

		int tp() const { return _tp; }
		void setTP(int tp) { _tp = std::min(tp, 1000); }

		int mp() const { return _mp; }
		void setMP(int mp) { _mp = std::min(mp, maximumMP()); }
			
		int maximumMP() const;

		struct AppliedAura {
			const Aura* aura = nullptr;
			int count = 0;
			std::chrono::microseconds time = 0_us;
			std::chrono::microseconds duration = 0_us;
			double baseTickDamage = 0.0;
			double tickCriticalHitChance = 0.0;
			Stats providedStats;
		};
		
		const std::unordered_map<std::pair<std::string, Actor*>, AppliedAura>& auras() const { return _auras; }
		
	private:
		const Configuration* const _configuration = nullptr;
		std::random_device* const _rng = nullptr;

		Actor* _pet = nullptr;
		Actor* _owner = nullptr;
		
		const Action* _command = nullptr;

		const Action* _comboAction = nullptr;
		std::chrono::microseconds _comboActionTime = -1_min;

		Stats _stats;
		Stats _baseStats;

		void _updateStats();

		std::chrono::microseconds _time = 0_us;
		std::chrono::microseconds _globalCooldownStartTime = -1_min;
		std::chrono::microseconds _lastAutoAttackTime = -1_min;

		const Action* _castAction = nullptr;
		Actor* _castTarget = nullptr;
		std::chrono::microseconds _castStartTime = -1_min;

		struct Cooldown {
			std::chrono::microseconds time = 0_us;
			std::chrono::microseconds duration = 0_us;
		};

		std::chrono::microseconds _animationLockEndTime = 0_us;

		int _tp = 1000;
		int _mp = 0;

		std::unordered_map<std::string, Cooldown> _cooldowns;
		std::unordered_map<std::pair<std::string, Actor*>, AppliedAura> _auras;

		SimulationStats _simulationStats;
		std::unordered_map<std::string, EffectSimulationStats> _effectSimulationStats;
		
		void _integrateAuraApplicationCountChange(const Aura* aura, int count);
		Damage _generateTickDamage(const AppliedAura& application) const;
};
