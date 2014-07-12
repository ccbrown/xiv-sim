#include "Ifrit.h"

#include "../Action.h"
#include "../Actor.h"
#include "../Aura.h"

namespace models {

Ifrit::Ifrit() : Base("ifrit") {
	struct SpellSpeedUp : Aura {
		SpellSpeedUp() : Aura("spell-speed-up") {
			_statsMultiplier.spellSpeed = 1.20;
		}
		virtual std::chrono::microseconds duration() const override { return 8_s; }
	};
	
	{
		struct Spell : Action {
			Spell() : Action("burning-strike") {}
			virtual int damage() const override { return 120; }
			virtual void resolution(Actor* source, Actor* target, bool isCritical) const override {
				std::uniform_real_distribution<double> distribution(0.0, 1.0);				
				if (distribution(source->rng()) < 0.20 && source->owner()) {
					source->owner()->applyAura(&spellSpeedUp, source);
				}
			}

			SpellSpeedUp spellSpeedUp;
		};
	
		_registerAction<Spell>();
	}

	{
		struct Spell : Action {
			Spell() : Action("inferno") {}
			virtual int damage() const override { return 200; }
			virtual std::chrono::microseconds cooldown() const override { return 300_s; }
			virtual void resolution(Actor* source, Actor* target, bool isCritical) const override {
				std::uniform_real_distribution<double> distribution(0.0, 1.0);				
				if (distribution(source->rng()) < 0.20 && source->owner()) {
					source->owner()->applyAura(&spellSpeedUp, source);
				}
			}
			
			SpellSpeedUp spellSpeedUp;
		};
	
		_registerAction<Spell>();
	}
}

std::chrono::microseconds Ifrit::globalCooldown(const Actor* actor) const {
	return _baseGlobalCooldown(actor);
}

DamageType Ifrit::_defaultDamageType() const {
	return DamageTypeBlunt;
}

std::chrono::microseconds Ifrit::_baseGlobalCooldown(const Actor* actor) const {
	return 3300_ms;
}

// TODO: this number may be wrong (was copied / pasted from monk)

double Ifrit::_basePotencyMultiplier(const Actor* actor) const {
	auto& stats = actor->stats();
	return 0.01 * (stats.weaponPhysicalDamage * (stats.strength * 0.00389 + stats.determination * 0.0008 + 0.01035) + (stats.strength * 0.08034) + (stats.determination * 0.02622));
}

double Ifrit::_baseAutoAttackDamage(const Actor* actor) const {
	auto& stats = actor->stats();
	return stats.weaponDelay / 3.0 * (stats.weaponPhysicalDamage * (stats.strength * 0.00408 + stats.determination * 0.00208 - 0.30991) + (stats.strength * 0.07149) + (stats.determination * 0.03443));
}

}