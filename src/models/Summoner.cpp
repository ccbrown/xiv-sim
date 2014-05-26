#include "Summoner.h"

#include "../Action.h"
#include "../Actor.h"

namespace models {

Summoner::Summoner() {
	{
		struct Spell : Action {
			struct DoT : Aura {
				DoT() : Aura("bio-dot") {}
				virtual std::chrono::microseconds duration() const override { return 18_s; }
				virtual int tickDamage() const { return 40; }
			};
			
			Spell() : Action("bio") {
				_targetAuras.push_back(new DoT());
			}
			virtual int mpCost() const override { return 106; }
		};
	
		_registerAction<Spell>();
	}

	{
		struct Spell : Action {
			Spell() : Action("ruin-ii") {}
			virtual int damage() const override { return 80; }
			virtual int mpCost() const override { return 133; }
		};
	
		_registerAction<Spell>();
	}
}

int Summoner::maximumMP(const Actor* actor) const {
	return 3012 + (actor->stats().piety - 214) * 7.25;
}

DamageType Summoner::_defaultDamageType() const {
	return DamageTypeMagic;
}

std::chrono::microseconds Summoner::_baseGlobalCooldown(const Actor* actor) const {
	auto& stats = actor->stats();
	auto gcd = std::chrono::duration<double>(2.49 - (stats.spellSpeed - 344) * (0.01 / 10.5));
	return std::chrono::duration_cast<std::chrono::microseconds>(gcd);
}

// TODO: these numbers are probably 100% wrong (they were copied / pasted from monk)

double Summoner::_basePotencyMultiplier(const Actor* actor) const {
	auto& stats = actor->stats();
	return 0.01 * (stats.weaponDamage * (stats.intelligence * 0.00389 + stats.determination * 0.0008 + 0.01035) + (stats.intelligence * 0.08034) + (stats.determination * 0.02622));
}

double Summoner::_baseAutoAttackDamage(const Actor* actor) const {
	auto& stats = actor->stats();
	return stats.weaponDelay / 3.0 * (stats.weaponDamage * (stats.intelligence * 0.00408 + stats.determination * 0.00208 - 0.30991) + (stats.intelligence * 0.07149) + (stats.determination * 0.03443));
}

}