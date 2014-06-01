#pragma once

#include "Actor.h"

#include <chrono>
#include <queue>
#include <random>
#include <string>
#include <memory>
#include <vector>
#include <unordered_set>

class Simulation {
	public:
		struct Configuration {
			std::chrono::microseconds length;
			std::vector<Actor::Configuration*> subjectConfigurations;
			Actor::Configuration* targetConfiguration = nullptr;
		};

		template <typename T>
		Simulation(const Configuration* configuration, T seed) : _configuration(configuration), _rng(seed) {
			for (auto& subjectConfiguration : configuration->subjectConfigurations) {
				auto subject = new Actor(subjectConfiguration, &_rng);
				if (!_subjects.empty()) {
					subject->addAlly(_subjects.front());
				}
				_subjects.push_back(subject);
				if (subject->pet()) {
					
					_subjects.push_back(subject->pet());
				}
			}
		
			_target = new Actor(configuration->targetConfiguration, &_rng);
			_subjects.push_back(_target);
		}

		~Simulation();

		void run();
		
		const std::vector<Actor*>& subjects() const { return _subjects; }

	private:
		const Configuration* const _configuration = nullptr;
		std::mt19937 _rng;

		std::vector<Actor*> _subjects;
		Actor* _target = nullptr;

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

		void _scheduleCheck(std::chrono::microseconds delay = 0_us);
		std::unordered_set<std::chrono::microseconds::rep> _scheduledChecks;

		void _checkActors();
		void _tick();
};
