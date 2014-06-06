#include "Dragoon.h"

#include "../Action.h"
#include "../Actor.h"
#include "../Aura.h"

namespace models {

Dragoon::Dragoon() : Base("dragoon") {
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
				Buff() : Aura("life-surge") {}
				virtual std::chrono::microseconds duration() const override { return 10_s; }
				virtual double additionalCriticalHitChance() const override { return 1.0; }
			};

			Skill() : Action("life-surge") {
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
				virtual double increasedDamage() const override { return 0.30; }
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
			virtual int tpRestoration() const override { return 500; }
		};
		
		_registerAction<Skill>();
	}

	{		
		struct Skill : Action {
			struct DoT : Aura {
				DoT() : Aura("fracture-dot") {}
				virtual std::chrono::microseconds duration() const override { return 18_s; }
				virtual int tickDamage() const override { return 20; }
			};

			Skill() : Action("fracture") {
				_targetAuras.push_back(new DoT());
			}
			virtual int damage() const override { return 100; }
			virtual int tpCost() const override { return 80; }
			virtual void resolution(Actor* source, Actor* target, bool isCritical) const override {
				source->dispelAura("life-surge", source);
			}
		};
		
		_registerAction<Skill>();
	}

	{
		struct Skill : Action {
			Skill() : Action("true-thrust") {}
			virtual int damage() const override { return 150; }
			virtual int tpCost() const override { return 70; }
			virtual void resolution(Actor* source, Actor* target, bool isCritical) const override {
				source->dispelAura("life-surge", source);
			}
		};
		
		_registerAction<Skill>();
	}

	{
		struct Skill : Action {
			Skill() : Action("vorpal-thrust-combo") {}
			virtual int damage() const override { return 200; }
			virtual int tpCost() const override { return 60; }
			virtual bool requirements(const Actor* source) const override {
				return source->comboAction() && source->comboAction()->identifier() == "true-thrust";
			}
			virtual void resolution(Actor* source, Actor* target, bool isCritical) const override {
				source->dispelAura("life-surge", source);
			}
		};
		
		_registerAction<Skill>();
	}

	{
		struct Skill : Action {
			Skill() : Action("full-thrust-combo") {}
			virtual int damage() const override { return 300; }
			virtual int tpCost() const override { return 60; }
			virtual bool requirements(const Actor* source) const override {
				return source->comboAction() && source->comboAction()->identifier() == "vorpal-thrust-combo";
			}
			virtual void resolution(Actor* source, Actor* target, bool isCritical) const override {
				source->dispelAura("life-surge", source);
			}
		};
		
		_registerAction<Skill>();
	}

	{
		struct Skill : Action {
			Skill() : Action("impulse-drive-rear") {}
			virtual int damage() const override { return 180; }
			virtual int tpCost() const override { return 70; }
			virtual void resolution(Actor* source, Actor* target, bool isCritical) const override {
				source->dispelAura("life-surge", source);
			}
		};
		
		_registerAction<Skill>();
	}

	{
		struct Skill : Action {
			struct Debuff : Aura {
				Debuff() : Aura("disembowel-combo") {}
				virtual std::chrono::microseconds duration() const override { return 30_s; }
				virtual void transformIncomingDamage(Damage* damage) const override {
					if (damage->type == DamageTypePiercing) {
						damage->amount *= 1.0 / 0.9;
					}
				}
			};
			
			Skill() : Action("disembowel-combo") {
				_targetAuras.push_back(new Debuff());
			}
			virtual int damage() const override { return 220; }
			virtual int tpCost() const override { return 60; }
			virtual bool requirements(const Actor* source) const override {
				return source->comboAction() && source->comboAction()->identifier() == "impulse-drive-rear";
			}
			virtual void resolution(Actor* source, Actor* target, bool isCritical) const override {
				source->dispelAura("life-surge", source);
			}
		};

		_registerAction<Skill>();
	}

	{
		struct Skill : Action {
			struct DoT : Aura {
				DoT() : Aura("chaos-thrust-dot") {}
				virtual std::chrono::microseconds duration() const override { return 30_s; }
				virtual int tickDamage() const override { return 30; }
			};
			
			Skill() : Action("chaos-thrust-combo") {
				_targetAuras.push_back(new DoT());
			}
			virtual int damage() const override { return 200; }
			virtual int tpCost() const override { return 60; }
			virtual bool requirements(const Actor* source) const override {
				return source->comboAction() && source->comboAction()->identifier() == "disembowel-combo";
			}
			virtual void resolution(Actor* source, Actor* target, bool isCritical) const override {
				source->dispelAura("life-surge", source);
			}
		};
	
		_registerAction<Skill>();
	}

	{
		struct Skill : Action {
			struct Buff : Aura {
				Buff() : Aura("heavy-thrust") {}
				virtual std::chrono::microseconds duration() const override { return 20_s; }
				virtual double increasedDamage() const override { return 0.15; }
			};
			
			Skill() : Action("heavy-thrust-flank") {
				_sourceAuras.push_back(new Buff());
			}
			virtual int damage() const override { return 170; }
			virtual int tpCost() const override { return 70; }
			virtual void resolution(Actor* source, Actor* target, bool isCritical) const override {
				source->dispelAura("life-surge", source);
			}
		};
	
		_registerAction<Skill>();
	}
	
	{
		struct Skill : Action {
			struct DoT : Aura {
				DoT() : Aura("phlebotomize-dot") {}
				virtual std::chrono::microseconds duration() const override { return 18_s; }
				virtual int tickDamage() const override { return 25; }
			};
			
			Skill() : Action("phlebotomize") {
				_targetAuras.push_back(new DoT());
			}
			virtual int damage() const override { return 170; }
			virtual int tpCost() const override { return 90; }
			virtual void resolution(Actor* source, Actor* target, bool isCritical) const override {
				source->dispelAura("life-surge", source);
			}
		};
	
		_registerAction<Skill>();
	}

	{
		struct Skill : Action {
			Skill() : Action("jump") {}
			virtual int damage(const Actor* source, const Actor* target) const override {
				return source->auraCount("power-surge", source) ? 300 : 200;
			}
			virtual std::chrono::microseconds cooldown() const override { return 40_s; }
			virtual void resolution(Actor* source, Actor* target, bool isCritical) const override {
				source->dispelAura("life-surge", source);
				source->dispelAura("power-surge", source);
			}
			virtual bool isOffGlobalCooldown() const override { return true; }
		};
	
		_registerAction<Skill>();
	}

	{
		struct Skill : Action {
			struct Buff : Aura {
				Buff() : Aura("power-surge") {}
				virtual std::chrono::microseconds duration() const override { return 10_s; }
			};
			
			Skill() : Action("power-surge") {
				_sourceAuras.push_back(new Buff());
			}
			virtual bool isOffGlobalCooldown() const override { return true; }
			virtual std::chrono::microseconds cooldown() const override { return 60_s; }
		};
	
		_registerAction<Skill>();
	}
	
	{
		struct Skill : Action {
			Skill() : Action("dragonfire-dive") {}
			virtual int damage() const override { return 250; }
			virtual std::chrono::microseconds cooldown() const override { return 180_s; }
			virtual void resolution(Actor* source, Actor* target, bool isCritical) const override {
				source->dispelAura("life-surge", source);
			}
			virtual bool isOffGlobalCooldown() const override { return true; }
		};
	
		_registerAction<Skill>();
	}

	{
		struct Skill : Action {
			Skill() : Action("leg-sweep") {}
			virtual int damage() const override { return 130; }
			virtual std::chrono::microseconds cooldown() const override { return 20_s; }
			virtual bool isOffGlobalCooldown() const override { return true; }
			virtual void resolution(Actor* source, Actor* target, bool isCritical) const override {
				source->dispelAura("life-surge", source);
			}
		};
		
		_registerAction<Skill>();
	}

	{
		struct Skill : Action {
			Skill() : Action("spineshatter-dive") {}
			virtual int damage() const override { return 170; }
			virtual std::chrono::microseconds cooldown() const override { return 90_s; }
			virtual bool isOffGlobalCooldown() const override { return true; }
			virtual void resolution(Actor* source, Actor* target, bool isCritical) const override {
				source->dispelAura("life-surge", source);
			}
		};

		_registerAction<Skill>();
	}
}

int Dragoon::maximumMP(const Actor* actor) const {
	return 0;
}

DamageType Dragoon::_defaultDamageType() const {
	return DamageTypePiercing;
}

std::chrono::microseconds Dragoon::_baseGlobalCooldown(const Actor* actor) const {
	auto& stats = actor->stats();
	auto gcd = std::chrono::duration<double>(2.5 - (stats.skillSpeed - 341) * (0.01 / 10.5));
	return std::chrono::duration_cast<std::chrono::microseconds>(gcd);
}

// http://valk.dancing-mad.com/

double Dragoon::_basePotencyMultiplier(const Actor* actor) const {
	auto& stats = actor->stats();
	return 0.01 * (stats.weaponPhysicalDamage * (stats.strength * 0.00389 + stats.determination * 0.0008 + 0.01035) + (stats.strength * 0.08034) + (stats.determination * 0.02622));
}

double Dragoon::_baseAutoAttackDamage(const Actor* actor) const {
	auto& stats = actor->stats();
	return stats.weaponDelay / 3.0 * (stats.weaponPhysicalDamage * (stats.strength * 0.00408 + stats.determination * 0.00208 - 0.30991) + (stats.strength * 0.07149) + (stats.determination * 0.03443));
}

}
