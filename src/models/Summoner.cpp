#include "Summoner.h"

#include "../Action.h"
#include "../Actor.h"

namespace models {

Summoner::Summoner() : Base("summoner") {
	{
		struct Spell : Action {
			struct DoT : Aura {
				DoT() : Aura("bio-dot") {}
				virtual std::chrono::microseconds duration() const override { return 18_s; }
				virtual int tickDamage() const override { return 40; }
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
			struct DoT : Aura {
				DoT() : Aura("bio-ii-dot") {}
				virtual std::chrono::microseconds duration() const override { return 30_s; }
				virtual int tickDamage() const override { return 30; }
			};
			
			Spell() : Action("bio-ii") {
				_targetAuras.push_back(new DoT());
			}
			virtual int mpCost() const override { return 186; }
		};
	
		_registerAction<Spell>();
	}

	{
		struct Spell : Action {
			struct DoT : Aura {
				DoT() : Aura("miasma-dot") {}
				virtual std::chrono::microseconds duration() const override { return 24_s; }
				virtual int tickDamage() const override { return 35; }
			};
			
			Spell() : Action("miasma") {
				_targetAuras.push_back(new DoT());
			}
			virtual int damage() const override { return 20; }
			virtual int mpCost() const override { return 133; }
		};
	
		_registerAction<Spell>();
	}

	{
		struct Spell : Action {
			struct DoT : Aura {
				DoT() : Aura("miasma-ii-dot") {}
				virtual std::chrono::microseconds duration() const override { return 15_s; }
				virtual int tickDamage() const override { return 10; }
			};
			
			Spell() : Action("miasma-ii") {
				_targetAuras.push_back(new DoT());
			}
			virtual int damage() const override { return 20; }
			virtual int mpCost() const override { return 186; }
		};
	
		_registerAction<Spell>();
	}

	{
		struct Spell : Action {
			Spell() : Action("ruin") {}
			virtual int damage() const override { return 80; }
			virtual std::chrono::microseconds castTime() const override { return 2500_ms; }
			virtual int mpCost() const override { return 79; }
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

	{
		struct Spell : Action {
			struct DoT : Aura {
				DoT() : Aura("shadow-flare-dot") {}
				virtual std::chrono::microseconds duration() const override { return 30_s; }
				virtual int tickDamage() const override { return 25; }
			};

			Spell() : Action("shadow-flare") {
				_targetAuras.push_back(new DoT());
			}
			virtual std::chrono::microseconds castTime() const override { return 3_s; }
			virtual int mpCost() const override { return 212; }
		};
	
		_registerAction<Spell>();
	}

	{
		struct Skill : Action {
			struct Buff : Aura {
				Buff() : Aura("raging-strikes") {}
				virtual std::chrono::microseconds duration() const override { return 20_s; }
				virtual double increasedDamage() const override { return 0.20; }
			};
			
			Skill() : Action("raging-strikes") {
				_sourceAuras.push_back(new Buff());
			}
			virtual bool isOffGlobalCooldown() const override { return true; }
			virtual std::chrono::microseconds cooldown() const override { return 180_s; }
		};
		
		_registerAction<Skill>();
	}

	{
		struct Skill : Action {
			struct Buff : Aura {
				Buff() : Aura("swiftcast") {}
				virtual std::chrono::microseconds duration() const override { return 10_s; }
				// implementation in Actor
			};
			
			Skill() : Action("swiftcast") {
				_sourceAuras.push_back(new Buff());
			}
			virtual bool isOffGlobalCooldown() const override { return true; }
			virtual std::chrono::microseconds cooldown() const override { return 60_s; }
		};
		
		_registerAction<Skill>();
	}

	{
		struct Spell : Action {
			Spell() : Action("aetherflow") {
				_sourceAuras.push_back(new Aetherflow());
				_sourceAuras.push_back(new Aetherflow());
				_sourceAuras.push_back(new Aetherflow());
			}
			virtual bool isOffGlobalCooldown() const override { return true; }
			virtual std::chrono::microseconds cooldown() const override { return 60_s; }
			virtual int mpRestoration(const Actor* subject) const override { return subject->maximumMP() * 0.20; }
		};

		_registerAction<Spell>();
	}

	{		
		struct Spell : Action {
			Spell() : Action("fester") {}
			virtual std::chrono::microseconds cooldown() const override { return 10_s; }
			virtual void resolution(Actor* source, Actor* target) const override {
				source->dispelAura("aetherflow", source);
			}
			virtual int damage(const Actor* source, const Actor* target) const override {
				return (target->auraCount("bio-dot", source) ? 100 : 0) + (target->auraCount("bio-ii-dot", source) ? 100 : 0) + (target->auraCount("miasma-dot", source) ? 100 : 0);
			}
			virtual bool requirements(const Actor* source) const override {
				return source->auraCount("aetherflow", source);
			}
			virtual bool isOffGlobalCooldown() const override { return true; }
		};
		
		_registerAction<Spell>();
	}

	{
		struct Spell : Action {
			struct Buff : Aura {
				Buff() : Aura("rouse") {}
				virtual std::chrono::microseconds duration() const override { return 20_s; }
				virtual double increasedDamage() const override { return 0.40; }
			} buff;

			Spell() : Action("rouse") {}
			virtual bool isOffGlobalCooldown() const override { return true; }
			virtual std::chrono::microseconds cooldown() const override { return 60_s; }
			virtual void resolution(Actor* source, Actor* target) const override {
				source->pet()->applyAura(&buff, source);
			}
		};

		_registerAction<Spell>();
	}

	{
		struct Spell : Action {
			struct Buff : Aura {
				Buff() : Aura("spur") {
					_statsMultiplier.strength = 1.40;
					_statsMultiplier.dexterity = 1.40;
					_statsMultiplier.intelligence = 1.40;
				}
				virtual std::chrono::microseconds duration() const override { return 20_s; }
			} buff;

			Spell() : Action("spur") {}
			virtual bool isOffGlobalCooldown() const override { return true; }
			virtual std::chrono::microseconds cooldown() const override { return 120_s; }
			virtual void resolution(Actor* source, Actor* target) const override {
				source->pet()->applyAura(&buff, source);
			}
		};

		_registerAction<Spell>();
	}
}

void Summoner::prepareForBattle(Actor* actor) const {
	actor->applyAura(&aetherflow, actor, 3);
}

int Summoner::maximumMP(const Actor* actor) const {
	return 3012 + (actor->stats().piety - 214) * 7.25;
}

DamageType Summoner::_defaultDamageType() const {
	return DamageTypeMagic;
}

std::chrono::microseconds Summoner::_baseGlobalCooldown(const Actor* actor) const {
	auto& stats = actor->stats();
	auto gcd = std::chrono::duration<double>(2.5 - (stats.spellSpeed - 341) * (0.01 / 10.5));
	return std::chrono::duration_cast<std::chrono::microseconds>(gcd);
}

// http://www.reddit.com/r/ffxiv/comments/1nxw7m/caster_damage_formula_testingstat_weights/

double Summoner::_basePotencyMultiplier(const Actor* actor) const {
	auto& stats = actor->stats();
	return 0.01 * (stats.weaponMagicDamage * stats.intelligence * 0.00587517 + stats.determination * 0.074377 + stats.intelligence * 0.077076);
}

// TODO: this number may be wrong (was copied / pasted from monk)

double Summoner::_baseAutoAttackDamage(const Actor* actor) const {
	auto& stats = actor->stats();
	return stats.weaponDelay / 3.0 * (stats.weaponPhysicalDamage * (stats.strength * 0.00408 + stats.determination * 0.00208 - 0.30991) + (stats.strength * 0.07149) + (stats.determination * 0.03443));
}

}