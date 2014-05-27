#include "Garuda.h"

#include "../Action.h"
#include "../Actor.h"

namespace models {

Garuda::Garuda() {
	{
		struct Spell : Action {
			Spell() : Action("wind-blade") {}
			virtual int damage() const override { return 100; }
			virtual std::chrono::microseconds castTime() const override { return 1_s; }
		};
	
		_registerAction<Spell>();
	}

	{
		struct Spell : Action {
			Spell() : Action("contagion") {}
			virtual std::chrono::microseconds cooldown() const override { return 60_s; }
			virtual void resolution(Actor* subject, Actor* target) const override {
				target->extendAura("bio-dot", subject->owner(), 15_s);
				target->extendAura("bio-ii-dot", subject->owner(), 15_s);
				target->extendAura("miasma-dot", subject->owner(), 15_s);
				target->extendAura("miasma-ii-dot", subject->owner(), 15_s);
			}
		};
	
		_registerAction<Spell>();
	}
}

std::chrono::microseconds Garuda::globalCooldown(const Actor* actor) const {
	return _baseGlobalCooldown(actor);
}

std::chrono::microseconds Garuda::autoAttackInterval(const Actor* actor) const {
	return std::chrono::microseconds::max();
}

DamageType Garuda::_defaultDamageType() const {
	return DamageTypeMagic;
}

std::chrono::microseconds Garuda::_baseGlobalCooldown(const Actor* actor) const {
	return 3300_ms;
}

// TODO: these numbers are probably 100% wrong (they were copied / pasted from monk)

double Garuda::_basePotencyMultiplier(const Actor* actor) const {
	auto& stats = actor->stats();
	return 0.01 * (stats.weaponDamage * (stats.intelligence * 0.00389 + stats.determination * 0.0008 + 0.01035) + (stats.intelligence * 0.08034) + (stats.determination * 0.02622));
}

double Garuda::_baseAutoAttackDamage(const Actor* actor) const {
	auto& stats = actor->stats();
	return stats.weaponDelay / 3.0 * (stats.weaponDamage * (stats.intelligence * 0.00408 + stats.determination * 0.00208 - 0.30991) + (stats.intelligence * 0.07149) + (stats.determination * 0.03443));
}

}