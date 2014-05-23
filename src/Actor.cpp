#include "Actor.h"

#include "Model.h"
#include "Rotation.h"

const Action* Actor::act(const Actor* target) const {
	return _configuration->rotation ? _configuration->rotation->nextAction(this, target) : nullptr;
}

Damage Actor::generatedDamage(const Action* action) const {
	return _configuration->model->generatedDamage(action, this);
}

Damage Actor::acceptedDamage(const Damage& incoming) const {
	return _configuration->model->acceptedDamage(incoming, this);
}

void Actor::advanceTime(const std::chrono::microseconds& time) {
	_time = time;
}

void Actor::triggerGlobalCooldown() {
	_globalCooldownStartTime = _time;
}

std::chrono::microseconds Actor::globalCooldownRemaining() const {
	// TODO: make sure this matches ffxiv for changes to skill / spell speed mid-gcd
	auto elapsed = _time - _globalCooldownStartTime;
	auto gcd = _configuration->model->globalCooldown(this);
	return elapsed < gcd ? gcd - elapsed : 0us;
}
