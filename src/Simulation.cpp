#include "Simulation.h"

#include "Action.h"
#include "Model.h"

#include <random>

Simulation::Simulation(const Configuration* configuration) : _configuration(configuration) {
	auto subject = new Actor(configuration->subjectConfiguration, &_rng);
	_subjects.push_back(subject);
	if (subject->pet()) {
		_subjects.push_back(subject->pet());
	}

	_target = new Actor(configuration->targetConfiguration, &_rng);
	_subjects.push_back(_target);
}

Simulation::~Simulation() {
	for (auto& subject : _subjects) {
		if (!subject->owner()) {
			delete subject;
		}
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

void Simulation::_checkActors() {
	for (auto& subject : _subjects) {
		if (subject == _target) { continue; }

		if (!subject->currentCast() && !subject->autoAttackDelayRemaining().count()) {
			auto damage = _target->acceptDamage(subject->performAutoAttack());
			subject->integrateDamageStats(damage, "auto-attack");
	
			_schedule([&] {
				_checkActors();
			}, subject->autoAttackDelayRemaining());
		}
		
		if (auto action = subject->act(_target)) {
			_resolveAction(action, subject, _target);
		}
	}
}

void Simulation::_tick() {
	for (auto& subject : _subjects) {
		subject->tick();		
	}

	_checkActors();

	_schedule([&] {
		_tick();
	}, 3_s);	
}

void Simulation::_resolveAction(const Action* action, Actor* subject, Actor* target) {
	if (action->castTime(subject).count()) {
		if (subject->beginCast(action, target)) {
			std::chrono::microseconds remaining;
			if (subject->currentCast(&remaining)) {
				_schedule([&] {
					_checkActors();
				}, remaining);
			}
		}
	} else if (subject->completeAction(action, target)) {
		if (!action->isOffGlobalCooldown()) {
			_schedule([&] {
				_checkActors();
			}, subject->globalCooldownRemaining());
		}

		_schedule([&] {
			_checkActors();
		}, subject->animationLockRemaining());

		_schedule([&] {
			_checkActors();
		});
	}
}
