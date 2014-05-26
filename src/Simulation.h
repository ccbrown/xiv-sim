#pragma once

#include "Actor.h"

#include <chrono>
#include <queue>
#include <map>
#include <random>
#include <string>
#include <memory>

class Simulation {
	public:
		struct Configuration {
			std::chrono::microseconds length;
			std::random_device* rng = nullptr;
			Actor::Configuration* subjectConfiguration = nullptr;
			Actor::Configuration* targetConfiguration = nullptr;
		};
		
		struct Stats {
			int count = 0;
			int criticalHits = 0;
			int totalDamageDealt = 0;
			
			Stats& operator+=(const Stats& other) {
				count += other.count;
				criticalHits += other.criticalHits;
				totalDamageDealt += other.totalDamageDealt;
				return *this;
			}			
		};
	
		Simulation(const Configuration* configuration)
			: _configuration(configuration)
			, _subject(configuration->subjectConfiguration, configuration->rng)
			, _target(configuration->targetConfiguration, configuration->rng)
		{}

		void run();
		
		const Stats& stats() const { return _stats; }
		const std::map<std::string, Stats>& statsByEffect() const { return _statsByEffect; }

	private:
		const Configuration* const _configuration = nullptr;
		Actor _subject;
		Actor _target;

		std::chrono::microseconds _time = 0_us;

		struct ScheduledFunction {
			ScheduledFunction(std::shared_ptr<std::function<void()>> function, std::chrono::microseconds time, uint64_t order)
				: function(function), time(time), order(order) {}
			
			std::shared_ptr<std::function<void()>> function;
			std::chrono::microseconds time;
			uint64_t order;
			
			bool operator<(const ScheduledFunction& other) const {
				if (time > other.time) {
					return true;
				} else if (time == other.time) {
					return order > other.order;
				}
				return false;
			}
		};
	
		std::priority_queue<ScheduledFunction> _scheduledFunctions;	
		uint64_t _nextOrder = 0;
		bool _shouldStop = false;

		void _advanceTime(std::chrono::microseconds time);

		void _schedule(const std::function<void()>& function, std::chrono::microseconds delay = 0_us);

		void _checkActors();
		void _tick();
		void _resolveAction(const Action* action, Actor* subject, Actor* target);
		
		Stats _damageStats(const Damage& damage);
		
		Stats _stats;
		std::map<std::string, Stats> _statsByEffect;
};
