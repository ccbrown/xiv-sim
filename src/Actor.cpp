#include "Actor.h"

#include "Aura.h"
#include "Model.h"
#include "Rotation.h"

const Action* Actor::act(const Actor* target) const {
	return _configuration->rotation ? _configuration->rotation->nextAction(this, target) : nullptr;
}

Damage Actor::generateDamage(const Action* action) {
	return _configuration->model->generateDamage(action, this);
}

Damage Actor::acceptDamage(const Damage& incoming) const {
	return _configuration->model->acceptDamage(incoming, this);
}

void Actor::advanceTime(const std::chrono::microseconds& time) {
	_time = time;

	for (auto it = _cooldowns.begin(); it != _cooldowns.end();) {
		if (_time - it->second.time >= it->second.duration) {
			it = _cooldowns.erase(it);
		} else {
			++it;
		}
	}

	for (auto it = _auras.begin(); it != _auras.end();) {
		if (_time - it->second.time >= it->second.duration) {
			it = _auras.erase(it);
		} else {
			++it;
		}
	}
}

Damage Actor::performAutoAttack() {
	_lastAutoAttackTime = _time;
	return _configuration->model->generateAutoAttackDamage(this);
}

void Actor::triggerGlobalCooldown() {
	_globalCooldownStartTime = _time;
}

int Actor::maximumMP() const {
	return _configuration->model->maximumMP(this);
}

std::chrono::microseconds Actor::autoAttackDelayRemaining() const {
	// TODO: make sure this matches ffxiv for changes to greased lightning between auto-attacks
	auto elapsed = _time - _lastAutoAttackTime;
	auto interval = _configuration->model->autoAttackInterval(this);
	return elapsed < interval ? interval - elapsed : 0_us;
}

std::chrono::microseconds Actor::globalCooldownRemaining() const {
	// TODO: make sure this matches ffxiv for changes to skill / spell speed mid-gcd
	auto elapsed = _time - _globalCooldownStartTime;
	auto gcd = _configuration->model->globalCooldown(this);
	return elapsed < gcd ? gcd - elapsed : 0_us;
}

void Actor::applyAura(Actor* source, Aura* aura) {
	for (auto& kv : _auras) {
		if (kv.second.aura->providesImmunity(aura)) {
			return;
		}
	}
	
	auto& application = _auras[aura->identifier()];

	application.aura = aura;
	application.time = _time;
	application.duration = aura->duration();
	
	application.baseTickDamage = source->_configuration->model->baseTickDamage(source, aura);
	application.tickCriticalHitChance = source->_configuration->model->tickCriticalHitChance(source);

	if (application.count < aura->maximumCount()) {
		++application.count;
	}
}

void Actor::dispelAura(const std::string& identifier) {
	_auras.erase(identifier);
}

int Actor::auraCount(const std::string& identifier) const {
	auto it = _auras.find(identifier);
	return it == _auras.end() ? 0 : it->second.count;
}

std::chrono::microseconds Actor::auraTimeRemaining(const std::string& identifier) const {
	auto it = _auras.find(identifier);
	return it == _auras.end() ? 0_us : (it->second.duration - (_time - it->second.time));
}

Damage Actor::generateTickDamage(const std::string& auraIdentifier) const {
	Damage ret;
	auto it = _auras.find(auraIdentifier);
	if (it != _auras.end()) {
		auto& application = it->second;

		std::uniform_real_distribution<double> distribution(0.0, 1.0);

		ret.isCritical = (distribution(*_rng) < application.tickCriticalHitChance);

		double amount = application.baseTickDamage;

		// TODO: this sway is a complete guess off the top of my head and should be researched
		amount *= 1.0 + (0.5 - distribution(*_rng)) * 0.1;

		if (ret.isCritical) {
			amount *= 1.5;
		}
		
		ret.amount = amount;
	}
	return ret;
}

double Actor::damageMultiplier() const {
	double ret = 1.0;
	for (auto& kv : _auras) {
		ret *= (1.0 + kv.second.aura->increasedDamage() * kv.second.count);
	}
	return ret;
}

double Actor::autoAttackSpeedMultiplier() const {
	double ret = 1.0;
	for (auto& kv : _auras) {
		ret *= (1.0 + kv.second.aura->increasedAutoAttackSpeed() * kv.second.count);
	}
	return ret;
}

void Actor::transformIncomingDamage(Damage* damage) const {
	for (auto& kv : _auras) {
		kv.second.aura->transformIncomingDamage(damage);
	}
}

double Actor::additionalCriticalHitChance() const {
	double ret = 0.0;
	for (auto& kv : _auras) {
		ret += kv.second.aura->additionalCriticalHitChance();
	}
	return ret;
}


void Actor::triggerCooldown(const std::string& identifier, std::chrono::microseconds duration) {
	auto& cooldown = _cooldowns[identifier];
	cooldown.duration = std::max((cooldown.time + cooldown.duration) - _time, duration);
	cooldown.time = _time;
}

std::chrono::microseconds Actor::cooldownRemaining(const std::string& identifier) const {
	auto it = _cooldowns.find(identifier);
	return it == _cooldowns.end() ? 0_us : ((it->second.time + it->second.duration) - _time);
}
