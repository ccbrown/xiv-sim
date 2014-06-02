#include "Bard.h"

#include "../Action.h"
#include "../Actor.h"
#include "../Aura.h"

namespace models {

Bard::Bard() : Base("bard") {
	{
		struct Skill : Action {
			struct SelfBuff : Aura {
				SelfBuff() : Aura("armys-paeon") {}
				virtual std::chrono::microseconds duration() const override { return std::chrono::microseconds::max(); }
				virtual double increasedDamage() const override { return -0.20; }
				virtual bool shouldCancel(Actor* actor, Actor* source, int count) const override {
					return actor->mp() < 133;
				}
				virtual void tick(Actor* actor, Actor* source, int count, bool isCritical) const override {
					actor->setMP(actor->mp() - 133);
					actor->setTP(actor->tp() + 30);
				}
				virtual void afterEffect(Actor* actor, Actor* source, int count) const override {
					for (auto ally : actor->allies()) {
						ally->dispelAura("armys-paeon", source);
					}
				}
			};

			struct AllyBuff : Aura {
				AllyBuff() : Aura("armys-paeon") {}
				virtual std::chrono::microseconds duration() const override { return std::chrono::microseconds::max(); }
				virtual void tick(Actor* actor, Actor* source, int count, bool isCritical) const override {
					actor->setTP(actor->tp() + 30);
				}
			};

			Skill() : Action("armys-paeon") {
				_sourceAuras.push_back(new SelfBuff());
			}
			virtual void resolution(Actor* source, Actor* target) const override {
				for (auto ally : source->allies()) {
					ally->applyAura(&allyBuff, source);
				}
			}
			virtual std::chrono::microseconds castTime() const override { return 3_s; }
				
			AllyBuff allyBuff;
		};

		_registerAction<Skill>();
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
			virtual std::chrono::microseconds cooldown() const override { return 120_s; }
		};
		
		_registerAction<Skill>();
	}
	
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
			virtual double criticalHitChance(const Actor* source, double base) const {
				return source->auraCount("heavier-shot", source) ? 1.0 : base;
			}
			virtual void resolution(Actor* source, Actor* target) const override {
				source->dispelAura("heavier-shot", source);
			}
		};
	
		_registerAction<Skill>();
	}

	{
		struct Skill : Action {
			Skill() : Action("bloodletter") {}
			virtual int damage() const override { return 150; }
			virtual bool isOffGlobalCooldown() const { return true; }
			virtual std::chrono::microseconds cooldown() const override { return 15_s; }
		};
	
		_registerAction<Skill>();
	}

	{
		struct Skill : Action {
			Skill() : Action("repelling-shot") {}
			virtual int damage() const override { return 80; }
			virtual bool isOffGlobalCooldown() const { return true; }
			virtual std::chrono::microseconds cooldown() const override { return 30_s; }
		};
	
		_registerAction<Skill>();
	}

	{
		struct Skill : Action {
			Skill() : Action("blunt-arrow") {}
			virtual int damage() const override { return 50; }
			virtual bool isOffGlobalCooldown() const { return true; }
			virtual std::chrono::microseconds cooldown() const override { return 30_s; }
		};
	
		_registerAction<Skill>();
	}

	{
		struct Skill : Action {
			struct DoT : Aura {
				DoT() : Aura("windbite-dot") {}
				virtual std::chrono::microseconds duration() const override { return 18_s; }
				virtual int tickDamage() const override { return 45; }
				virtual void tick(Actor* actor, Actor* source, int count, bool isCritical) const {
					std::uniform_real_distribution<double> distribution(0.0, 1.0);				
					if (distribution(source->rng()) < 0.5) {
						source->endCooldown("bloodletter");
					}
				}
			};
			
			Skill() : Action("windbite") {
				_targetAuras.push_back(new DoT());
			}
			virtual int damage() const override { return 60; }
			virtual int tpCost() const override { return 80; }
		};
	
		_registerAction<Skill>();
	}

	{
		struct Skill : Action {
			struct DoT : Aura {
				DoT() : Aura("venomous-bite-dot") {}
				virtual std::chrono::microseconds duration() const override { return 18_s; }
				virtual int tickDamage() const override { return 35; }
				virtual void tick(Actor* actor, Actor* source, int count, bool isCritical) const {
					std::uniform_real_distribution<double> distribution(0.0, 1.0);				
					if (distribution(source->rng()) < 0.5) {
						source->endCooldown("bloodletter");
					}
				}
			};
			
			Skill() : Action("venomous-bite") {
				_targetAuras.push_back(new DoT());
			}
			virtual int damage() const override { return 100; }
			virtual int tpCost() const override { return 80; }
		};
	
		_registerAction<Skill>();
	}

	{
		struct Skill : Action {
			struct Buff : Aura {
				Buff() : Aura("hawks-eye") {
					_statsMultiplier.dexterity = 1.15;
				}
				virtual std::chrono::microseconds duration() const override { return 20_s; }
			};

			Skill() : Action("hawks-eye") {
				_sourceAuras.push_back(new Buff());
			}
			virtual bool isOffGlobalCooldown() const override { return true; }
			virtual std::chrono::microseconds cooldown() const override { return 90_s; }
		};
		
		_registerAction<Skill>();
	}

	{
		struct Skill : Action {
			struct DoT : Aura {
				DoT() : Aura("flaming-arrow-dot") {}
				virtual std::chrono::microseconds duration() const override { return 30_s; }
				virtual int tickDamage() const override { return 35; }
			};
			
			Skill() : Action("flaming-arrow") {
				_targetAuras.push_back(new DoT());
			}
			virtual std::chrono::microseconds cooldown() const override { return 60_s; }
			virtual bool isOffGlobalCooldown() const override { return true; }
		};
	
		_registerAction<Skill>();
	}

	{
		struct Skill : Action {
			struct Buff : Aura {
				Buff() : Aura("heavier-shot") {}
				virtual std::chrono::microseconds duration() const override { return 10_s; }
			} heavierShot;

			Skill() : Action("heavy-shot") {}
			virtual int damage() const override { return 150; }
			virtual int tpCost() const override { return 60; }
			virtual void resolution(Actor* source, Actor* target) const override {
				std::uniform_real_distribution<double> distribution(0.0, 1.0);				
				if (distribution(source->rng()) < 0.2) {
					source->applyAura(&heavierShot, source);
				}
			}
		};
	
		_registerAction<Skill>();
	}

	{
		struct Skill : Action {
			struct Buff : Aura {
				Buff() : Aura("barrage") {}
				virtual int additionalStrikesPerAutoAttack() const override { return 2; }
				virtual std::chrono::microseconds duration() const override { return 10_s; }
			};

			Skill() : Action("barrage") {
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
	return 0.01 * (stats.weaponPhysicalDamage * (stats.dexterity * 0.00389 + stats.determination * 0.0008 + 0.01035) + (stats.dexterity * 0.08034) + (stats.determination * 0.02622));
}

double Bard::_baseAutoAttackDamage(const Actor* actor) const {
	auto& stats = actor->stats();
	return stats.weaponDelay / 3.0 * (stats.weaponPhysicalDamage * (stats.dexterity * 0.00408 + stats.determination * 0.00208 - 0.30991) + (stats.dexterity * 0.07149) + (stats.determination * 0.03443));
}

}