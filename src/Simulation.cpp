#include "Simulation.h"

#include "Action.h"
#include "Model.h"

#include "common.h"

Simulation::~Simulation() {
	std::vector<Actor*> toDelete;
	for (auto& subject : _subjects) {
		if (!subject->owner()) {
			toDelete.push_back(subject);
		}
	}
	for (auto& actor : toDelete) {
		delete actor;
	}
}

void Simulation::run() {
	PortableUniformIntDistribution<std::chrono::microseconds::rep> distribution(0, 3000000);
	std::chrono::microseconds dotTickOffset(distribution(_rng));

	for (auto& subject : _subjects) {
		subject->prepareForBattle();
	}

	_schedule([&] {
		_shouldStop = true;
	}, _configuration->length);

	_schedule([&] {
		_checkActors();
	});

	_schedule([&] {
		_tick();
	}, dotTickOffset);

	while (!_shouldStop && !_scheduledFunctions.empty()) {
		auto& top = _scheduledFunctions.top();
		_advanceTime(top.time);
		auto function = top.function;
		_scheduledFunctions.pop();
		function->operator()();

		for (auto& subject : _subjects) {
			if (subject == _target) { continue; }
			auto delay = subject->timeUntilNextTimeOfInterest();
			if (delay != std::chrono::microseconds::max()) {
				_scheduleCheck(delay);
			}
		}
	}
}

void Simulation::_advanceTime(std::chrono::microseconds time) {
	_time = time;
	for (auto& subject : _subjects) {
		subject->advanceTime(_time);
	}
}

void Simulation::_schedule(const std::function<void()>& function, std::chrono::microseconds delay) {
	_scheduledFunctions.push(ScheduledFunction(std::make_shared<std::function<void()>>(function), _time + delay, _nextOrder++));
}

void Simulation::_scheduleCheck(std::chrono::microseconds delay) {
	if (_scheduledChecks.insert((_time + delay).count()).second) {
		_schedule([&] { _checkActors(); }, delay);
	}
}

void Simulation::_checkActors() {
	_scheduledChecks.erase(_time.count());

	for (auto& subject : _subjects) {
		if (subject == _target) { continue; }
		
		subject->act(_target);

		if (!subject->currentCast() && subject->isAutoAttacking() && !subject->autoAttackDelayRemaining().count()) {
			for (int i = 0; i < subject->strikesPerAutoAttack(); ++i) {
				auto damage = _target->acceptDamage(subject->performAutoAttack());
				subject->integrateDamageStats(damage, "auto-attack");
			}
		}
	}
}

void Simulation::_tick() {
	for (auto& subject : _subjects) {
		subject->pretick();		
	}

	for (auto& subject : _subjects) {
		subject->tick();		
	}

	_checkActors();

	_schedule([&] { _tick(); }, 3_s);	
}
