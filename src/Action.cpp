#include "Action.h"

#include "Actor.h"
#include "Aura.h"

Action::~Action() {
	for (auto& aura : _subjectAuras) { delete aura; }
	for (auto& aura : _targetAuras) { delete aura; }
}

bool Action::resolve(Actor* subject, Actor* target) const {
	if (subject->currentCast()) { return false; }
	
	if (!isOffGlobalCooldown() && subject->isOnGlobalCooldown()) { return false; }

	if (subject->tp() < tpCost() || subject->mp() < mpCost()) { return false; }

	if (cooldown().count()) {
		if (subject->cooldownRemaining(identifier()).count()) { return false; }

		subject->triggerCooldown(identifier(), cooldown());
	}

	if (!isOffGlobalCooldown()) {
		subject->triggerGlobalCooldown();
	}

	subject->setTP(subject->tp() - tpCost() + tpRestoration());
	subject->setMP(subject->mp() - mpCost() + mpRestoration(subject));

	Damage damage;

	if (this->damage(subject, target)) {
		damage = target->acceptDamage(subject->generateDamage(this, target));
	}

	for (auto& kv : subject->auras()) {
		if (int count = dispelsSubjectAura(kv.second.aura)) {
			subject->dispelAura(kv.second.aura->identifier(), kv.first.second, count);
			break;
		}
	}

	for (auto& kv : target->auras()) {
		if (int count = dispelsTargetAura(kv.second.aura)) {
			target->dispelAura(kv.second.aura->identifier(), kv.first.second, count);
			break;
		}
	}

	for (auto& aura : subjectAuras()) {
		subject->applyAura(aura, subject);
	}

	for (auto& aura : targetAuras()) {
		target->applyAura(aura, subject);
	}
	
	resolution(subject, target);

	subject->integrateDamageStats(damage, identifier().c_str());

	return true;
}