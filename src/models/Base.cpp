#include "Base.h"

#include "../Action.h"
#include "../Actor.h"

#include <random>

namespace models {

Base::~Base() {
	for (auto& kv : _actions) {
		delete kv.second;
	}
}

const Action* Base::action(const char* identifier) const {
	auto it = _actions.find(identifier);
	return it == _actions.end() ? nullptr : it->second;
}

std::chrono::microseconds Base::globalCooldown(const Actor* actor) const {
	return std::chrono::duration_cast<std::chrono::microseconds>(_baseGlobalCooldown(actor) * actor->globalCooldownMultiplier());
}

Damage Base::generateDamage(const Action* action, Actor* actor) const {
	auto& stats = actor->stats();
	
	Damage ret;
	double criticalHitChance = action->criticalHitChance((stats.criticalHitRate * 0.0697 - 18.437) / 100.0) + actor->additionalCriticalHitChance();

	std::uniform_real_distribution<double> distribution(0.0, 1.0);

	ret.isCritical = (distribution(actor->rng()) < criticalHitChance);

	double amount = action->damage() * _basePotencyMultiplier(actor);
	amount *= actor->damageMultiplier();

	// TODO: this sway is a complete guess off the top of my head and should be researched
	amount *= 1.0 + (0.5 - distribution(actor->rng())) * 0.1;

	if (ret.isCritical) {
		amount *= 1.5;
	}
	
	ret.amount = amount;
	ret.type = _defaultDamageType();

	return ret;
}

Damage Base::generateAutoAttackDamage(Actor* actor) const {
	auto& stats = actor->stats();

	Damage ret;
	double criticalHitChance = (stats.criticalHitRate * 0.0697 - 18.437) / 100.0 + actor->additionalCriticalHitChance();

	std::uniform_real_distribution<double> distribution(0.0, 1.0);

	ret.isCritical = (distribution(actor->rng()) < criticalHitChance);

	double amount = _baseAutoAttackDamage(actor);
	amount *= actor->damageMultiplier();

	// TODO: this sway is a complete guess off the top of my head and should be researched
	amount *= 1.0 + (0.5 - distribution(actor->rng())) * 0.1;

	if (ret.isCritical) {
		amount *= 1.5;
	}

	ret.amount = amount;
	ret.type = _defaultDamageType();

	return ret;
}

Damage Base::acceptDamage(const Damage& incoming, const Actor* actor) const {
	Damage ret = incoming;
	actor->transformIncomingDamage(&ret);
	ret.amount = round(ret.amount);
	return ret;
}

std::chrono::microseconds Base::autoAttackInterval(const Actor* actor) const {
	auto& stats = actor->stats();
	auto interval = std::chrono::duration<double>(stats.weaponDelay / actor->autoAttackSpeedMultiplier());
	return std::chrono::duration_cast<std::chrono::microseconds>(interval);
}

double Base::baseTickDamage(const Actor* source, const Aura* aura) const {
	double ret = aura->tickDamage() * _basePotencyMultiplier(source);
	ret *= source->damageMultiplier();
	return ret;
}

double Base::tickCriticalHitChance(const Actor* source) const {
	auto& stats = source->stats();
	return (stats.criticalHitRate * 0.0697 - 18.437) / 100.0 + source->additionalCriticalHitChance();
}

}
