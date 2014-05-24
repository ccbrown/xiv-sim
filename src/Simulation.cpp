#include "Simulation.h"

#include "Action.h"
#include "Model.h"

#include <random>

void Simulation::run() {
	std::random_device generator;
	std::uniform_int_distribution<std::chrono::microseconds::rep> distribution(0, 3000000);
	
	std::chrono::microseconds dotTickOffset(distribution(generator));

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
	_subject.advanceTime(_time);
	_target.advanceTime(_time);
}

void Simulation::_schedule(const std::function<void()>& function, std::chrono::microseconds delay) {
	_scheduledFunctions.push(ScheduledFunction(std::make_shared<std::function<void()>>(function), _time + delay, _nextOrder++));
}

void Simulation::_checkActors() {
	if (!_subject.autoAttackDelayRemaining().count()) {
		auto damage = _target.acceptDamage(_subject.performAutoAttack());

		auto stats = _damageStats(damage);
		_stats += stats;
		_statsByEffect["auto-attack"] += stats;

		_schedule([&] {
			_checkActors();
		}, _subject.autoAttackDelayRemaining());
	}
	
	if (auto action = _subject.act(&_target)) {
		_resolveAction(action, &_subject, &_target);
	}
}

void Simulation::_tick() {
	_subject.setTP(std::min(_subject.tp() + 60, 1000));
	
	for (auto& kv : _target.auras()) {
		if (!kv.second.aura->tickDamage()) { continue; }
		
		auto damage = _target.acceptDamage(_target.generateTickDamage(kv.first));

		auto stats = _damageStats(damage);
		_stats += stats;
		_statsByEffect[kv.first] += stats;
	}
	
	_checkActors();

	_schedule([&] {
		_tick();
	}, 3_s);	
}

void Simulation::_resolveAction(const Action* action, Actor* subject, Actor* target) {	
	if (!action->isOffGlobalCooldown() && subject->isOnGlobalCooldown()) { return; }

	if (subject->tp() < action->tpCost()) { return; }

	if (action->cooldown().count()) {
		if (subject->cooldownRemaining(action->identifier()).count()) { return; }

		subject->triggerCooldown(action->identifier(), action->cooldown());
	}
	
	if (!action->isOffGlobalCooldown()) {
		subject->triggerGlobalCooldown();
		_schedule([&] {
			_checkActors();
		}, subject->globalCooldownRemaining());
	}

	subject->setTP(subject->tp() - action->tpCost() + action->tpRestoration());

	Damage damage;

	if (action->damage()) {
		damage = target->acceptDamage(subject->generateDamage(action));
	}

	for (auto& kv : subject->auras()) {
		if (action->dispelsSubjectAura(kv.second.aura)) {
			subject->dispelAura(kv.second.aura->identifier());
			break;
		}
	}

	for (auto& kv : target->auras()) {
		if (action->dispelsTargetAura(kv.second.aura)) {
			target->dispelAura(kv.second.aura->identifier());
			break;
		}
	}

	for (auto& aura : action->subjectAuras()) {
		subject->applyAura(subject, aura);
	}

	for (auto& aura : action->targetAuras()) {
		target->applyAura(subject, aura);
	}

	auto stats = _damageStats(damage);
	if (subject == &_subject) {
		_stats += stats;
	}
	_statsByEffect[action->identifier()] += stats;

	_schedule([&] {
		_checkActors();
	});
}

Simulation::Stats Simulation::_damageStats(const Damage& damage) {
	Stats ret;
	ret.count = 1;
	ret.criticalHits = damage.isCritical ? 1 : 0;
	ret.totalDamageDealt = damage.amount;
	return ret;
}
