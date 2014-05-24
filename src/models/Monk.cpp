#include "Monk.h"

#include "../Action.h"
#include "../Actor.h"

#include <mutex>
#include <random>

namespace models {

Monk::Monk() {
	static std::once_flag initializationFlag;
	std::call_once(initializationFlag, InitializeActions);
}

Damage Monk::generateDamage(const Action* action, const Actor* actor) const {
	auto& stats = actor->stats();
	
	Damage ret;
	double criticalHitChance = action->criticalHitChance((stats.criticalHitRate * 0.0697 - 18.437) / 100.0) + actor->additionalCriticalHitChance();

	std::random_device generator;
	std::uniform_real_distribution<double> distribution(0.0, 1.0);

	ret.isCritical = (distribution(generator) < criticalHitChance);

	double amount = action->damage() / 100.0 * (stats.weaponDamage * (stats.strength * 0.00389 + stats.determination * 0.0008 + 0.01035) + (stats.strength * 0.08034) + (stats.determination * 0.02622));
	amount *= actor->damageMultiplier();

	// TODO: this sway is a complete guess off the top of my head and should be researched
	amount *= 1.0 + (0.5 - distribution(generator)) * 0.1;

	if (ret.isCritical) {
		amount *= 1.5;
	}
	
	ret.amount = amount;
	ret.type = DamageTypeBlunt;

	return ret;
}

Damage Monk::generateAutoAttackDamage(const Actor* actor) const {
	auto& stats = actor->stats();

	Damage ret;
	double criticalHitChance = (stats.criticalHitRate * 0.0697 - 18.437) / 100.0 + actor->additionalCriticalHitChance();

	std::random_device generator;
	std::uniform_real_distribution<double> distribution(0.0, 1.0);

	ret.isCritical = (distribution(generator) < criticalHitChance);

	double amount = stats.weaponDelay / 3.0 * (stats.weaponDamage * (stats.strength * 0.00408 + stats.determination * 0.00208 - 0.30991) + (stats.strength * 0.07149) + (stats.determination * 0.03443));
	amount *= actor->damageMultiplier();

	// TODO: this sway is a complete guess off the top of my head and should be researched
	amount *= 1.0 + (0.5 - distribution(generator)) * 0.1;

	if (ret.isCritical) {
		amount *= 1.5;
	}

	ret.amount = amount;
	ret.type = DamageTypeBlunt;

	return ret;
}

Damage Monk::acceptDamage(const Damage& incoming, const Actor* actor) const {
	Damage ret = incoming;
	actor->transformIncomingDamage(&ret);
	ret.amount = round(ret.amount);
	return ret;
}

std::chrono::microseconds Monk::globalCooldown(const Actor* actor) const {
	auto& stats = actor->stats();
	auto gcd = std::chrono::duration<double>(2.49 - (stats.skillSpeed - 344) * (0.01 / 10.5));
	return std::chrono::duration_cast<std::chrono::microseconds>(gcd);
}

std::chrono::microseconds Monk::autoAttackInterval(const Actor* actor) const {
	auto& stats = actor->stats();
	auto interval = std::chrono::duration<double>(stats.weaponDelay / actor->autoAttackSpeedMultiplier());
	return std::chrono::duration_cast<std::chrono::microseconds>(interval);
}

double Monk::baseTickDamage(const Actor* source, const Aura* aura) const {
	auto& stats = source->stats();
	double ret = aura->tickDamage() / 100.0 * (stats.weaponDamage * (stats.strength * 0.00389 + stats.determination * 0.0008 + 0.01035) + (stats.strength * 0.08034) + (stats.determination * 0.02622));
	ret *= source->damageMultiplier();
	return ret;
}

double Monk::tickCriticalHitChance(const Actor* source) const {
	auto& stats = source->stats();
	return (stats.criticalHitRate * 0.0697 - 18.437) / 100.0 + source->additionalCriticalHitChance();
}

const Action* Monk::FistsOfFire = nullptr;
const Action* Monk::DragonKickFlankOpoOpo = nullptr;
const Action* Monk::BootshineRear = nullptr;
const Action* Monk::BootshineRearOpoOpo = nullptr;
const Action* Monk::TwinSnakesFlank = nullptr;
const Action* Monk::TrueStrikeRear = nullptr;
const Action* Monk::SnapPunchFlank = nullptr;
const Action* Monk::DemolishRear = nullptr;
const Action* Monk::TouchOfDeath = nullptr;
const Action* Monk::SteelPeak = nullptr;
const Action* Monk::HowlingFist = nullptr;
const Action* Monk::InternalRelease = nullptr;
const Action* Monk::BloodForBlood = nullptr;
const Action* Monk::Invigorate = nullptr;
const Action* Monk::PerfectBalance = nullptr;

void Monk::InitializeActions() {
	{
		static const struct Skill : Action {			
			struct Buff : Aura {
				Buff() : Aura("fists-of-fire") {}
				virtual std::chrono::microseconds duration() const override { return std::chrono::microseconds::max(); }
				virtual double increasedDamage() const override { return 0.05; }
			};
	
			Skill() : Action("fists-of-fire") {
				_subjectAuras.push_back(new Buff());
			}
			virtual bool isOffGlobalCooldown() const { return true; }
		} fistsOfFire;
		
		FistsOfFire = &fistsOfFire;
	}

	struct RaptorForm : Aura {
		RaptorForm() : Aura("raptor-form") {}
		virtual std::chrono::microseconds duration() const override { return 10s; }
	};

	struct CoeurlForm : Aura {
		CoeurlForm() : Aura("coeurl-form") {}
		virtual std::chrono::microseconds duration() const override { return 10s; }
	};

	struct OpoOpoForm : Aura {
		OpoOpoForm() : Aura("opo-opo-form") {}
		virtual std::chrono::microseconds duration() const override { return 10s; }
	};

	struct GreasedLightning : Aura {
		GreasedLightning() : Aura("greased-lightning") {}
		virtual std::chrono::microseconds duration() const override { return 12s; }
		virtual int maximumCount() const override { return 3; }
		virtual double increasedDamage() const override { return 0.09; }
		virtual double increasedAutoAttackSpeed() const { return 0.05; }
	};

	{
		static const struct Skill : Action {
			struct Debuff : Aura {
				Debuff() : Aura("dragon-kick") {}
				virtual std::chrono::microseconds duration() const override { return 15s; }
				virtual void transformIncomingDamage(Damage* damage) const override {
					if (damage->type == DamageTypeBlunt) {
						damage->amount *= 1.0 / 0.9;
					}
				}
			};
			
			Skill() : Action("dragon-kick-flank-opo-opo") {
				_targetAuras.push_back(new Debuff());
				_subjectAuras.push_back(new RaptorForm());
			}
			virtual int damage() const override { return 150; }
			virtual int tpCost() const override { return 60; }
			virtual bool dispelsSubjectAura(Aura* aura) const override { return aura->identifier() == "opo-opo-form"; }
		} dragonKickFlankOpoOpo;

		DragonKickFlankOpoOpo = &dragonKickFlankOpoOpo;
	}
	
	{
		static const struct Skill : Action {
			Skill() : Action("bootshine-rear") {
				_subjectAuras.push_back(new RaptorForm());
			}
			virtual int damage() const override { return 150; }
			virtual int tpCost() const override { return 60; }
			virtual bool dispelsSubjectAura(Aura* aura) const override { return aura->identifier() == "opo-opo-form"; }
		} bootshineRear;
		
		BootshineRear = &bootshineRear;
	}

	{
		static const struct Skill : Action {
			Skill() : Action("bootshine-rear-opo-opo") {
				_subjectAuras.push_back(new RaptorForm());
			}
			virtual int damage() const override { return 150; }
			virtual int tpCost() const override { return 60; }
			virtual double criticalHitChance(double base) const override { return 1.0; }
			virtual bool dispelsSubjectAura(Aura* aura) const override { return aura->identifier() == "opo-opo-form"; }
		} bootshineRearOpoOpo;
		
		BootshineRearOpoOpo = &bootshineRearOpoOpo;
	}

	{
		static const struct Skill : Action {
			struct Buff : Aura {
				Buff() : Aura("twin-snakes") {}
				virtual std::chrono::microseconds duration() const override { return 15s; }
				virtual double increasedDamage() const override { return 0.10; }
			};
			
			Skill() : Action("twin-snakes-flank") {
				_subjectAuras.push_back(new Buff());
				_subjectAuras.push_back(new CoeurlForm());
			}
			virtual int damage() const override { return 140; }
			virtual int tpCost() const override { return 60; }
			virtual bool dispelsSubjectAura(Aura* aura) const override { return aura->identifier() == "raptor-form"; }
		} twinSnakesFlank;
		
		TwinSnakesFlank = &twinSnakesFlank;
	}
	
	{
		static const struct Skill : Action {
			Skill() : Action("true-strike-rear") {
				_subjectAuras.push_back(new CoeurlForm());
			}
			virtual int damage() const override { return 190; }
			virtual int tpCost() const override { return 50; }
			virtual bool dispelsSubjectAura(Aura* aura) const override { return aura->identifier() == "raptor-form"; }
		} trueStrikeRear;
		
		TrueStrikeRear = &trueStrikeRear;
	}

	{
		static const struct Skill : Action {
			Skill() : Action("snap-punch-flank") {
				_subjectAuras.push_back(new OpoOpoForm());
				_subjectAuras.push_back(new GreasedLightning());
			}
			virtual int damage() const override { return 180; }
			virtual int tpCost() const override { return 50; }
			virtual bool dispelsSubjectAura(Aura* aura) const override { return aura->identifier() == "coeurl-form"; }
		} snapPunchFlank;
		
		SnapPunchFlank = &snapPunchFlank;
	}

	{
		static const struct Skill : Action {
			struct DoT : Aura {
				DoT() : Aura("demolish-dot") {}
				virtual std::chrono::microseconds duration() const override { return 18s; }
				virtual int tickDamage() const { return 40; }
			};
			
			Skill() : Action("demolish-rear") {
				_subjectAuras.push_back(new OpoOpoForm());
				_subjectAuras.push_back(new GreasedLightning());
				_targetAuras.push_back(new DoT());
			}
			virtual int damage() const override { return 70; }
			virtual int tpCost() const override { return 50; }
			virtual bool dispelsSubjectAura(Aura* aura) const override { return aura->identifier() == "coeurl-form"; }
		} demolishRear;
	
		DemolishRear = &demolishRear;
	}

	{
		struct DoT : Aura {
			DoT() : Aura("touch-of-death-dot") {}
			virtual std::chrono::microseconds duration() const override { return 30s; }
			virtual int tickDamage() const { return 25; }
		};
		
		static const struct Skill : Action {
			Skill() : Action("touch-of-death") {
				_targetAuras.push_back(new DoT());
			}
			virtual int damage() const override { return 20; }
			virtual int tpCost() const override { return 80; }
		} touchOfDeath;
		
		TouchOfDeath = &touchOfDeath;
	}

	{
		static const struct Skill : Action {
			Skill() : Action("steel-peak") {}
			virtual int damage() const override { return 150; }
			virtual bool isOffGlobalCooldown() const { return true; }
			virtual std::chrono::microseconds cooldown() const { return 40s; }
		} steelPeak;
		
		SteelPeak = &steelPeak;
	}

	{
		static const struct Skill : Action {
			Skill() : Action("howling-fist") {}
			virtual int damage() const override { return 170; }
			virtual bool isOffGlobalCooldown() const { return true; }
			virtual std::chrono::microseconds cooldown() const { return 60s; }
		} howlingFist;
		
		HowlingFist = &howlingFist;
	}

	{
		static const struct Skill : Action {
			struct Buff : Aura {
				Buff() : Aura("internal-release") {}
				virtual std::chrono::microseconds duration() const override { return 15s; }
				virtual double additionalCriticalHitChance() const override { return 0.30; }
			};

			Skill() : Action("internal-release") {
				_subjectAuras.push_back(new Buff());
			}
			virtual bool isOffGlobalCooldown() const { return true; }
			virtual std::chrono::microseconds cooldown() const { return 60s; }
		} internalRelease;
		
		InternalRelease = &internalRelease;
	}
	
	{
		static const struct Skill : Action {
			struct Buff : Aura {
				Buff() : Aura("blood-for-blood") {}
				virtual std::chrono::microseconds duration() const override { return 20s; }
				virtual double increasedDamage() const override { return 0.10; }
			};
			
			Skill() : Action("blood-for-blood") {
				_subjectAuras.push_back(new Buff());
			}
			virtual bool isOffGlobalCooldown() const { return true; }
			virtual std::chrono::microseconds cooldown() const { return 80s; }
		} bloodForBlood;
		
		BloodForBlood = &bloodForBlood;
	}

	{
		static const struct Skill : Action {
			Skill() : Action("invigorate") {}
			virtual bool isOffGlobalCooldown() const { return true; }
			virtual std::chrono::microseconds cooldown() const { return 120s; }
			virtual int tpRestoration() const override { return 400; }
		} invigorate;
		
		Invigorate = &invigorate;
	}
	
	{
		static const struct Skill : Action {
			struct Buff : Aura {
				Buff() : Aura("perfect-balance") {}
				virtual std::chrono::microseconds duration() const override { return 10s; }
			};
			
			Skill() : Action("perfect-balance") {
				_subjectAuras.push_back(new Buff());
			}
			virtual bool isOffGlobalCooldown() const { return true; }
			virtual std::chrono::microseconds cooldown() const { return 240s; }
		} perfectBalance;
		
		PerfectBalance = &perfectBalance;
	}
}

}