#include "Action.h"

#include "Actor.h"
#include "Aura.h"

Action::~Action() {
	for (auto& aura : _sourceAuras) { delete aura; }
	for (auto& aura : _targetAuras) { delete aura; }
}

bool Action::isReady(const Actor* source) const {
	if (cooldown().count() && source->cooldownRemaining(identifier()).count()) { return false; }
			
	return requirements(source);
}

bool Action::isUsable(const Actor* source) const {
	if (source->currentCast()) { return false; }
	
	if (!isOffGlobalCooldown() && source->isOnGlobalCooldown()) { return false; }

	if (source->tp() < tpCost() || source->mp() < mpCost()) { return false; }

	return isReady(source);
}

bool Action::resolve(Actor* source, Actor* target) const {
	if (!isUsable(source)) { return false; }

	if (cooldown().count()) {
		source->triggerCooldown(identifier(), cooldown());
	}

	if (!isOffGlobalCooldown()) {
		source->triggerGlobalCooldown();
	}

	source->setTP(source->tp() - tpCost() + tpRestoration());
	source->setMP(source->mp() - mpCost() + mpRestoration(source));

	Damage damage;

	if (this->damage(source, target)) {
		damage = target->acceptDamage(source->generateDamage(this, target));
	}

	for (auto& kv : source->auras()) {
		if (int count = dispelsSourceAura(kv.second.aura)) {
			source->dispelAura(kv.second.aura->identifier(), kv.first.second, count);
			break;
		}
	}

	for (auto& kv : target->auras()) {
		if (int count = dispelsTargetAura(kv.second.aura)) {
			target->dispelAura(kv.second.aura->identifier(), kv.first.second, count);
			break;
		}
	}

	for (auto& aura : sourceAuras()) {
		source->applyAura(aura, source);
	}

	for (auto& aura : targetAuras()) {
		target->applyAura(aura, source);
	}
	
	resolution(source, target);

	source->integrateDamageStats(damage, identifier().c_str());

	return true;
}