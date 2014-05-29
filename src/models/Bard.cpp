#include "Bard.h"

#include "../Action.h"
#include "../Actor.h"
#include "../Aura.h"

namespace models {

Bard::Bard() {
	{
		struct Skill : Action {
			struct Buff : Aura {
				Buff() : Aura("straight-shot") {}
				virtual std::chrono::microseconds duration() const override { return 20_s; }
				virtual double additionalCriticalHitChance() const override { return 0.1; }
			};
			
			Skill() : Action("straight-shot") {
				_sourceAuras.push_back(new Buff());
			}
			virtual int damage() const override { return 140; }
			virtual int tpCost() const override { return 60; }
		};
	
		_registerAction<Skill>();
	}

	{
		struct Skill : Action {
			struct Buff : Aura {
				Buff() : Aura("internal-release") {}
				virtual std::chrono::microseconds duration() const override { return 15_s; }
				virtual double additionalCriticalHitChance() const override { return 0.10; }
			};

			Skill() : Action("internal-release") {
				_sourceAuras.push_back(new Buff());
			}
			virtual bool isOffGlobalCooldown() const override { return true; }
			virtual std::chrono::microseconds cooldown() const override { return 60_s; }
		};
		
		_registerAction<Skill>();
	}
	
	{
		struct Skill : Action {
			struct Buff : Aura {
				Buff() : Aura("blood-for-blood") {}
				virtual std::chrono::microseconds duration() const override { return 20_s; }
				virtual double increasedDamage() const override { return 0.10; }
			};
			
			Skill() : Action("blood-for-blood") {
				_sourceAuras.push_back(new Buff());
			}
			virtual bool isOffGlobalCooldown() const override { return true; }
			virtual std::chrono::microseconds cooldown() const override { return 80_s; }
		};
		
		_registerAction<Skill>();
	}

	{
		struct Skill : Action {
			Skill() : Action("invigorate") {}
			virtual bool isOffGlobalCooldown() const override { return true; }
			virtual std::chrono::microseconds cooldown() const override { return 120_s; }
			virtual int tpRestoration() const override { return 400; }
		};
		
		_registerAction<Skill>();
	}
}

int Bard::maximumMP(const Actor* actor) const {
	return 3012 + (actor->stats().piety - 214) * 7.25;
}

DamageType Bard::_defaultDamageType() const {
	return DamageTypePiercing;
}

std::chrono::microseconds Bard::_baseGlobalCooldown(const Actor* actor) const {
	auto& stats = actor->stats();
	auto gcd = std::chrono::duration<double>(2.5 - (stats.skillSpeed - 341) * (0.01 / 10.5));
	return std::chrono::duration_cast<std::chrono::microseconds>(gcd);
}

// http://valk.dancing-mad.com/

double Bard::_basePotencyMultiplier(const Actor* actor) const {
	auto& stats = actor->stats();
	return 0.01 * (stats.weaponDamage * (stats.dexterity * 0.00389 + stats.determination * 0.0008 + 0.01035) + (stats.dexterity * 0.08034) + (stats.determination * 0.02622));
}

double Bard::_baseAutoAttackDamage(const Actor* actor) const {
	auto& stats = actor->stats();
	return stats.weaponDelay / 3.0 * (stats.weaponDamage * (stats.dexterity * 0.00408 + stats.determination * 0.00208 - 0.30991) + (stats.dexterity * 0.07149) + (stats.determination * 0.03443));
}

}