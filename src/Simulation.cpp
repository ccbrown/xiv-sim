#include "Simulation.h"

#include "Action.h"
#include "Model.h"

void Simulation::run() {
	_schedule([&] {
		_shouldStop = true;
	}, _configuration->length);
	
	_schedule([&] {
		_checkActors();
	});
	
	while (!_shouldStop && !_scheduledFunctions.empty()) {
		auto& top = _scheduledFunctions.top();
		_advanceTime(top.time);
		auto function = top.function;
		_scheduledFunctions.pop();
		function->operator()();
	}
}

void Simulation::_advanceTime(std::chrono::microseconds time) {
	_time = time;
	_subject.advanceTime(_time);
	_target.advanceTime(_time);
}

void Simulation::_schedule(const std::function<void()>& function, std::chrono::microseconds delay) {
	_scheduledFunctions.push(ScheduledFunction(std::make_shared<std::function<void()>>(function), _time + delay, _nextOrder++));
}

void Simulation::_checkActors() {
	if (auto action = _subject.act(&_target)) {
		assert(!_subject.isOnGlobalCooldown() || action->isAllowedDuringGlobalCooldown());
		_resolveAction(action, &_subject, &_target);
	}
}

void Simulation::_resolveAction(const Action* action, Actor* subject, Actor* target) {	
	auto damage = target->acceptedDamage(subject->generatedDamage(action));

	Stats stats;
	stats.totalDamageDealt = damage.amount;

	if (subject == &_subject) {
		_stats += stats;
	}
	_statsByEffect[action->identifier()] += stats;

	if (action->triggersGlobalCooldown()) {
		subject->triggerGlobalCooldown();
		_schedule([&] {
			_checkActors();
		}, subject->globalCooldownRemaining());
	} else {
		_schedule([&] {
			_checkActors();
		});
	}
}
