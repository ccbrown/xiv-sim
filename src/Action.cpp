#include "Action.h"

#include "Actor.h"
#include "Aura.h"

Action::~Action() {
	for (auto& aura : _sourceAuras) { delete aura; }
	for (auto& aura : _targetAuras) { delete aura; }
}

bool Action::isReady(const Actor* source) const {
	if (cooldown().count() && source->cooldownRemaining(identifierHash()).count()) { return false; }
			
	return requirements(source);
}

bool Action::isUsable(const Actor* source) const {
	if (source->currentCast()) {  return false; }
	
	if (!isOffGlobalCooldown() && source->globalCooldownRemaining().count()) { return false; }
		
	if (source->animationLockRemaining().count()) { return false; }

	if (source->tp() < tpCost() || source->mp() < mpCost(source)) { return false; }

	return isReady(source);
}

bool Action::resolve(Actor* source, Actor* target) const {
	if (!isUsable(source)) { return false; }

	if (cooldown().count()) {
		source->triggerCooldown(identifierHash(), cooldown());
	}

	if (!isOffGlobalCooldown()) {
		source->triggerGlobalCooldown();
	}
	
	source->triggerAnimationLock();

	source->setTP(source->tp() - tpCost() + tpRestoration());
	source->setMP(source->mp() - mpCost(source) + mpRestoration(source));

	Damage damage;

	if (this->damage(source, target)) {
		damage = target->acceptDamage(source->generateDamage(this, target));
	}

	resolution(source, target, damage.isCritical);

	for (auto& aura : sourceAuras()) {
		source->applyAura(aura, source);
	}

	for (auto& aura : targetAuras()) {
		target->applyAura(aura, source);
	}

	source->integrateDamageStats(damage, identifier().c_str());

	return true;
}