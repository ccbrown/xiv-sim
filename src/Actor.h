#pragma once

#include "common.h"
#include "Aura.h"
#include "Damage.h"

#include <chrono>
#include <random>
#include <string>
#include <utility>
#include <map>
#include <unordered_map>
#include <unordered_set>

#include <stdint.h>

class Action;
class Model;
class Rotation;

class Actor {
	public:
		struct Stats {
			int weaponPhysicalDamage = 0;
			int weaponMagicDamage = 0;
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
			uint64_t count = 0;
			uint64_t criticalHits = 0;
			uint64_t damageDealt = 0;
			
			EffectSimulationStats& operator+=(const EffectSimulationStats& other) {
				count += other.count;
				criticalHits += other.criticalHits;
				damageDealt += other.damageDealt;
				return *this;
			}
		};

		struct SimulationStats {
			uint64_t damageDealt = 0;

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

		Actor(const Configuration* configuration, std::mt19937* rng);
		~Actor();

		const Configuration* configuration() const { return _configuration; }

		FNV1AHash identifierHash() const { return _identifierHash; }
		const std::string& identifier() const { return _configuration->identifier; }

		void prepareForBattle();
		void act(Actor* target);
		void pretick();
		void tick();
		
		const std::chrono::microseconds& time() const { return _time; }
		
		Actor* pet() { return _pet; }
		const Actor* pet() const { return _pet; }
			
		const std::unordered_set<Actor*>& allies() { return _allies; }
		void addAlly(Actor* actor);
			
		void setCommand(const Action* action) { _command = action; }
		const Action* command() const { return _command; }

		Actor* owner() { return _owner; }
		const Actor* owner() const { return _owner; }

		std::mt19937& rng() const { return *_rng; }
		const Model* model() const { return _configuration->model; }

		const Stats& stats() const { return _stats; }
		const SimulationStats& simulationStats() const { return _simulationStats; }
		const std::unordered_map<std::string, EffectSimulationStats>& effectSimulationStats() const { return _effectSimulationStats; }

		void integrateDamageStats(const Damage& damage, const char* effect);

		Damage generateDamage(const Action* action, const Actor* target);
		Damage acceptDamage(const Damage& incoming) const;

		void advanceTime(const std::chrono::microseconds& time);
		Damage performAutoAttack();

		const Action* comboAction() const;

		bool isAutoAttacking() const { return _isAutoAttacking; }
		void stopAutoAttack() { _isAutoAttacking = false; }

		std::chrono::microseconds autoAttackDelayRemaining() const;
		std::chrono::microseconds globalCooldown() const;
		std::chrono::microseconds globalCooldownRemaining() const;
		std::chrono::microseconds animationLockRemaining() const;
		
		std::chrono::microseconds timeUntilNextTimeOfInterest() const;

		void applyAura(const Aura* aura, Actor* source, int count = 1);
		int dispelAura(FNV1AHash identifierHash, const Actor* source, int count = 1);
		void extendAura(FNV1AHash identifierHash, const Actor* source, const std::chrono::microseconds& extension);

		int auraCount(FNV1AHash identifierHash, const Actor* source) const;
		std::chrono::microseconds auraTimeRemaining(FNV1AHash identifierHash, const Actor* source) const;

		double damageMultiplier() const;
		double autoAttackSpeedMultiplier() const;
		void transformIncomingDamage(Damage* damage) const;
		double additionalCriticalHitChance() const;
		
		int strikesPerAutoAttack() const;
		
		double globalCooldownMultiplier() const;

		void triggerCooldown(FNV1AHash identifierHash, std::chrono::microseconds duration);
		void endCooldown(FNV1AHash identifierHash);
		std::chrono::microseconds cooldownRemaining(FNV1AHash identifierHash) const;

		const Action* currentCast(std::chrono::microseconds* remaining = nullptr, Actor** target = nullptr) const;

		void triggerGlobalCooldown();
		void triggerAnimationLock(std::chrono::microseconds duration);

		int tp() const { return _tp; }
		void setTP(int tp) { _tp = std::max(std::min(tp, 1000), 0); }

		int mp() const { return _mp; }
		void setMP(int mp) { _mp = std::max(std::min(mp, maximumMP()), 0); }
			
		int maximumMP() const;

	private:
		const Configuration* const _configuration = nullptr;
		const FNV1AHash _identifierHash;
		std::mt19937* const _rng = nullptr;
		
		bool _isAutoAttacking = true;

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

		std::unordered_map<FNV1AHash, Cooldown> _cooldowns;

		struct AuraApplication {
			const Aura* aura = nullptr;
			Actor* source = nullptr;
			int count = 0;
			std::chrono::microseconds time = 0_us;
			std::chrono::microseconds duration = 0_us;
			double baseTickDamage = 0.0;
			double tickCriticalHitChance = 0.0;
			Stats providedStats;
		};

		struct AppliedAura {
			bool isSharedBetweenSources = false;
			std::map<FNV1AHash, AuraApplication> applications;
		};

		std::map<FNV1AHash, AppliedAura> _auras;

		SimulationStats _simulationStats;
		std::unordered_map<std::string, EffectSimulationStats> _effectSimulationStats;
		
		void _integrateAuraApplicationCountChange(const Aura* aura, int before, int after);
		Damage _generateTickDamage(const AuraApplication& application) const;
		
		std::unordered_set<Actor*> _allies;

		bool _beginCast(const Action* action, Actor* target);
		bool _executeAction(const Action* action, Actor* target);
};
