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

// http://www.reddit.com/r/ffxiv/comments/1nxw7m/caster_damage_formula_testingstat_weights/

double Garuda::_basePotencyMultiplier(const Actor* actor) const {
	auto& stats = actor->stats();
	return 0.01 * (stats.weaponDamage * stats.intelligence * 0.00587517 + stats.determination * 0.074377 + stats.intelligence * 0.077076);
}

}