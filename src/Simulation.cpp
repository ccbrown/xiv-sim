#include "Simulation.h"

#include "Action.h"
#include "Model.h"

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
	std::uniform_int_distribution<std::chrono::microseconds::rep> distribution(0, 3000000);
	std::chrono::microseconds dotTickOffset(distribution(_rng));

	_schedule([&] {
		_shouldStop = true;
	}, _configuration->length);

	_schedule([&] {
		_tick();
	}, dotTickOffset);

	_schedule([&] {
		_checkActors();
	});

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
	if (_time == time) { return; }

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

		if (!subject->currentCast() && !subject->autoAttackDelayRemaining().count()) {
			for (int i = 0; i < subject->strikesPerAutoAttack(); ++i) {
				auto damage = _target->acceptDamage(subject->performAutoAttack());
				subject->integrateDamageStats(damage, "auto-attack");
			}
		}
		
		if (auto action = subject->act(_target)) {
			if (action->castTime(subject).count()) {
				subject->beginCast(action, _target);
			} else {
				subject->executeAction(action, _target);
			}
		}
	}
}

void Simulation::_tick() {
	for (auto& subject : _subjects) {
		subject->tick();		
	}

	_checkActors();

	_schedule([&] { _tick(); }, 3_s);	
}
