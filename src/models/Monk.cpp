#include "Monk.h"

#include "../Action.h"
#include "../Actor.h"
#include "../Aura.h"

namespace models {

Monk::Monk() {
	{
		struct Skill : Action {			
			struct Buff : Aura {
				Buff() : Aura("fists-of-fire") {}
				virtual std::chrono::microseconds duration() const override { return std::chrono::microseconds::max(); }
				virtual double increasedDamage() const override { return 0.05; }
			};
	
			Skill() : Action("fists-of-fire") {
				_sourceAuras.push_back(new Buff());
			}
			virtual bool isOffGlobalCooldown() const override { return true; }
		};
		
		_registerAction<Skill>();
	}

	struct RaptorForm : Aura {
		RaptorForm() : Aura("raptor-form") {}
		virtual std::chrono::microseconds duration() const override { return 10_s; }
	};

	struct CoeurlForm : Aura {
		CoeurlForm() : Aura("coeurl-form") {}
		virtual std::chrono::microseconds duration() const override { return 10_s; }
	};

	struct OpoOpoForm : Aura {
		OpoOpoForm() : Aura("opo-opo-form") {}
		virtual std::chrono::microseconds duration() const override { return 10_s; }
	};

	struct GreasedLightning : Aura {
		GreasedLightning() : Aura("greased-lightning") {}
		virtual std::chrono::microseconds duration() const override { return 12_s; }
		virtual int maximumCount() const override { return 3; }
		virtual double increasedDamage() const override { return 0.09; }
		virtual double increasedAutoAttackSpeed() const override { return 0.05; }
		virtual double reducedGlobalCooldown() const override { return 0.05; }
	};

	{
		struct Skill : Action {
			struct Debuff : Aura {
				Debuff() : Aura("dragon-kick") {}
				virtual std::chrono::microseconds duration() const override { return 15_s; }
				virtual void transformIncomingDamage(Damage* damage) const override {
					if (damage->type == DamageTypeBlunt) {
						damage->amount *= 1.0 / 0.9;
					}
				}
			};
			
			Skill() : Action("dragon-kick-flank-opo-opo") {
				_targetAuras.push_back(new Debuff());
				_sourceAuras.push_back(new RaptorForm());
			}
			virtual int damage() const override { return 150; }
			virtual int tpCost() const override { return 60; }
			virtual void resolution(Actor* source, Actor* target) const override {
				source->dispelAura("opo-opo-form", source);
			}
			virtual bool requirements(const Actor* source) const override {
				return source->auraCount("opo-opo-form", source) || source->auraCount("perfect-balance", source);
			}
		};

		_registerAction<Skill>();
	}
	
	{
		struct Skill : Action {
			Skill() : Action("bootshine-rear") {
				_sourceAuras.push_back(new RaptorForm());
			}
			virtual int damage() const override { return 150; }
			virtual int tpCost() const override { return 60; }
			virtual void resolution(Actor* source, Actor* target) const override {
				source->dispelAura("opo-opo-form", source);
			}
		};
		
		_registerAction<Skill>();
	}

	{
		struct Skill : Action {
			Skill() : Action("bootshine-rear-opo-opo") {
				_sourceAuras.push_back(new RaptorForm());
			}
			virtual int damage() const override { return 150; }
			virtual int tpCost() const override { return 60; }
			virtual double criticalHitChance(const Actor* source, double base) const override { return 1.0; }
			virtual bool requirements(const Actor* source) const override {
				return source->auraCount("opo-opo-form", source) || source->auraCount("perfect-balance", source);
			}
			virtual void resolution(Actor* source, Actor* target) const override {
				source->dispelAura("opo-opo-form", source);
			}
		};
		
		_registerAction<Skill>();
	}

	{
		struct Skill : Action {
			struct Buff : Aura {
				Buff() : Aura("twin-snakes") {}
				virtual std::chrono::microseconds duration() const override { return 15_s; }
				virtual double increasedDamage() const override { return 0.10; }
			};
			
			Skill() : Action("twin-snakes-flank") {
				_sourceAuras.push_back(new Buff());
				_sourceAuras.push_back(new CoeurlForm());
			}
			virtual int damage() const override { return 140; }
			virtual int tpCost() const override { return 60; }
			virtual bool requirements(const Actor* source) const override {
				return source->auraCount("raptor-form", source) || source->auraCount("perfect-balance", source);
			}
			virtual void resolution(Actor* source, Actor* target) const override {
				source->dispelAura("raptor-form", source);
			}
		};

		_registerAction<Skill>();
	}
	
	{
		struct Skill : Action {
			Skill() : Action("true-strike-rear") {
				_sourceAuras.push_back(new CoeurlForm());
			}
			virtual int damage() const override { return 190; }
			virtual int tpCost() const override { return 50; }
			virtual bool requirements(const Actor* source) const override {
				return source->auraCount("raptor-form", source) || source->auraCount("perfect-balance", source);
			}
			virtual void resolution(Actor* source, Actor* target) const override {
				source->dispelAura("raptor-form", source);
			}
		};
		
		_registerAction<Skill>();
	}

	{
		struct Skill : Action {
			Skill() : Action("snap-punch-flank") {
				_sourceAuras.push_back(new OpoOpoForm());
				_sourceAuras.push_back(new GreasedLightning());
			}
			virtual int damage() const override { return 180; }
			virtual int tpCost() const override { return 50; }
			virtual bool requirements(const Actor* source) const override {
				return source->auraCount("coeurl-form", source) || source->auraCount("perfect-balance", source);
			}
			virtual void resolution(Actor* source, Actor* target) const override {
				source->dispelAura("coeurl-form", source);
			}
		};
		
		_registerAction<Skill>();
	}

	{
		struct Skill : Action {
			struct DoT : Aura {
				DoT() : Aura("demolish-dot") {}
				virtual std::chrono::microseconds duration() const override { return 18_s; }
				virtual int tickDamage() const override { return 40; }
			};
			
			Skill() : Action("demolish-rear") {
				_sourceAuras.push_back(new OpoOpoForm());
				_sourceAuras.push_back(new GreasedLightning());
				_targetAuras.push_back(new DoT());
			}
			virtual int damage() const override { return 70; }
			virtual int tpCost() const override { return 50; }
			virtual bool requirements(const Actor* source) const override {
				return source->auraCount("coeurl-form", source) || source->auraCount("perfect-balance", source);
			}
			virtual void resolution(Actor* source, Actor* target) const override {
				source->dispelAura("coeurl-form", source);
			}
		};
	
		_registerAction<Skill>();
	}

	{
		struct DoT : Aura {
			DoT() : Aura("touch-of-death-dot") {}
			virtual std::chrono::microseconds duration() const override { return 30_s; }
			virtual int tickDamage() const override { return 25; }
		};
		
		struct Skill : Action {
			Skill() : Action("touch-of-death") {
				_targetAuras.push_back(new DoT());
			}
			virtual int damage() const override { return 20; }
			virtual int tpCost() const override { return 80; }
		};
		
		_registerAction<Skill>();
	}

	{
		struct Skill : Action {
			Skill() : Action("steel-peak") {}
			virtual int damage() const override { return 150; }
			virtual bool isOffGlobalCooldown() const override { return true; }
			virtual std::chrono::microseconds cooldown() const override { return 40_s; }
		};
		
		_registerAction<Skill>();
	}

	{
		struct Skill : Action {
			Skill() : Action("howling-fist") {}
			virtual int damage() const override { return 170; }
			virtual bool isOffGlobalCooldown() const override { return true; }
			virtual std::chrono::microseconds cooldown() const override { return 60_s; }
		};
		
		_registerAction<Skill>();
	}

	{
		struct Skill : Action {
			struct Buff : Aura {
				Buff() : Aura("internal-release") {}
				virtual std::chrono::microseconds duration() const override { return 15_s; }
				virtual double additionalCriticalHitChance() const override { return 0.30; }
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
	
	{
		struct Skill : Action {
			struct Buff : Aura {
				Buff() : Aura("perfect-balance") {}
				virtual std::chrono::microseconds duration() const override { return 10_s; }
				virtual bool providesImmunity(const Aura* aura) const override {
					return aura->identifier() == "opo-opo-form" || aura->identifier() == "raptor-form" || aura->identifier() == "coeurl-form";
				}
			};
			
			Skill() : Action("perfect-balance") {
				_sourceAuras.push_back(new Buff());
			}
			virtual bool isOffGlobalCooldown() const override { return true; }
			virtual std::chrono::microseconds cooldown() const override { return 240_s; }
			virtual void resolution(Actor* source, Actor* target) const override {
				source->dispelAura("raptor-form", source);
				source->dispelAura("opo-opo-form", source);
				source->dispelAura("coeurl-form", source);
			}
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
		};
		
		_registerAction<Skill>();
	}
}

int Monk::maximumMP(const Actor* actor) const {
	return 0;
}

DamageType Monk::_defaultDamageType() const {
	return DamageTypeBlunt;
}

std::chrono::microseconds Monk::_baseGlobalCooldown(const Actor* actor) const {
	auto& stats = actor->stats();
	auto gcd = std::chrono::duration<double>(2.5 - (stats.skillSpeed - 341) * (0.01 / 10.5));
	return std::chrono::duration_cast<std::chrono::microseconds>(gcd);
}

// http://valk.dancing-mad.com/

double Monk::_basePotencyMultiplier(const Actor* actor) const {
	auto& stats = actor->stats();
	return 0.01 * (stats.weaponDamage * (stats.strength * 0.00389 + stats.determination * 0.0008 + 0.01035) + (stats.strength * 0.08034) + (stats.determination * 0.02622));
}

double Monk::_baseAutoAttackDamage(const Actor* actor) const {
	auto& stats = actor->stats();
	return stats.weaponDelay / 3.0 * (stats.weaponDamage * (stats.strength * 0.00408 + stats.determination * 0.00208 - 0.30991) + (stats.strength * 0.07149) + (stats.determination * 0.03443));
}

}