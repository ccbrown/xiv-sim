#include "Ninja.h"

#include "../Action.h"
#include "../Actor.h"
#include "../Aura.h"

namespace models {

Ninja::Ninja() : Base("ninja") {
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
			virtual int tpRestoration() const override { return 400; }
		};

		_registerAction<Skill>();
	}

	{
		struct Skill : Action {
			Skill() : Action("kiss-of-the-wasp") {
				_sourceAuras.push_back(new KissDamageBuff());
			}
		};

		_registerAction<Skill>();
	}

	{
		struct Skill : Action {
			Skill() : Action("kiss-of-the-viper") {
				_sourceAuras.push_back(new KissDamageBuff());
			}
		};

		_registerAction<Skill>();
	}

	{
		struct Skill : Action {
			Skill() : Action("spinning-edge") {}
			virtual int damage() const override { return 150; }
			virtual int tpCost() const override { return 60; }
		};

		_registerAction<Skill>();
	}

	{
		struct Skill : Action {
			Skill() : Action("gust-slash-combo") {}
			virtual int damage() const override { return 200; }
			virtual int tpCost() const override { return 50; }
			virtual bool requirements(const Actor* source) const override {
				return source->comboAction() && source->comboAction()->identifier() == "spinning-edge";
			}
			virtual void resolution(Actor* source, Actor* target, bool isCritical) const override {
				source->dispelAura("spinning-edge", source);
			}
		};

		_registerAction<Skill>();
	}

	{
		struct Skill : Action {
			Skill() : Action("aeolian-edge-combo") {}
			virtual int damage() const override { return 320; }
			virtual int tpCost() const override { return 60; }
			virtual bool requirements(const Actor* source) const override {
				return source->comboAction() && source->comboAction()->identifier() == "gust-slash-combo";
			}
			virtual void resolution(Actor* source, Actor* target, bool isCritical) const override {
				source->dispelAura("gust-slash-combo", source);
			}
		};

		_registerAction<Skill>();
	}

	{
		struct Skill : Action {
			struct Debuff : Aura {
				Debuff() : Aura("dancing-edge") {}
				virtual std::chrono::microseconds duration() const override { return 20_s; }
				virtual void transformIncomingDamage(Damage* damage) const override {
					if (damage->type == DamageTypeSlashing) {
						damage->amount *= 1.0 / 0.9;
					}
				}
			};

			Skill() : Action("dancing-edge-combo") {
				_targetAuras.push_back(new Debuff());
			}
			virtual int damage() const override { return 260; }
			virtual int tpCost() const override { return 50; }
			virtual bool requirements(const Actor* source) const override {
				return source->comboAction() && source->comboAction()->identifier() == "gust-slash-combo";
			}
			virtual void resolution(Actor* source, Actor* target, bool isCritical) const override {
				source->dispelAura("gust-slash-combo", source);
			}
		};

		_registerAction<Skill>();
	}

	{
		struct DoT : Aura {
			DoT() : Aura("shadow-fang-dot") {}
			virtual std::chrono::microseconds duration() const override { return 18_s; }
			virtual int tickDamage() const override { return 40; }
		};

		struct Skill : Action {
			Skill() : Action("shadow-fang-combo") {
				_targetAuras.push_back(new DoT());
			}
			virtual int damage() const override { return 200; }
			virtual int tpCost() const override { return 70; }
			virtual bool requirements(const Actor* source) const override {
				return source->comboAction() && source->comboAction()->identifier() == "spinning-edge";
			}
			virtual void resolution(Actor* source, Actor* target, bool isCritical) const override {
				source->dispelAura("spinning-edge", source);
			}
		};

		_registerAction<Skill>();
	}

	{
		struct DoT : Aura {
			DoT() : Aura("mutilate-dot") {}
			virtual std::chrono::microseconds duration() const override { return 30_s; }
			virtual int tickDamage() const override { return 30; }
		};

		struct Skill : Action {
			Skill() : Action("mutilate") {
				_targetAuras.push_back(new DoT());
			}
			virtual int damage() const override { return 60; }
			virtual int tpCost() const override { return 80; }
		};

		_registerAction<Skill>();
	}

	{
		struct Skill : Action {
			Skill() : Action("mug") {}
			virtual int damage() const override { return 140; }
			virtual bool isOffGlobalCooldown() const override { return true; }
			virtual std::chrono::microseconds cooldown() const override { return 90_s; }
		};

		_registerAction<Skill>();
	}

	{
		struct Skill : Action {
			Skill() : Action("jugulate") {}
			virtual int damage() const override { return 80; }
			virtual bool isOffGlobalCooldown() const override { return true; }
			virtual std::chrono::microseconds cooldown() const override { return 30_s; }
		};

		_registerAction<Skill>();
	}

	{
		struct Skill : Action {
			Skill() : Action("death-blossom") {}
			virtual int damage() const override { return 100; }
			virtual int tpCost() const override { return 120; }
		};

		_registerAction<Skill>();
	}

	{
		struct Skill : Action {
			struct Debuff : Aura {
				Debuff() : Aura("trick-attack-debuff") {}
				virtual std::chrono::microseconds duration() const override { return 10_s; }
				virtual void transformIncomingDamage(Damage* damage) const override {
					damage->amount *= 1.0 / 0.9;
				}
			};

			Skill() : Action("trick-attack") {
				_targetAuras.push_back(new Debuff());
			}
			virtual int damage() const override { return 400; }
			virtual bool isOffGlobalCooldown() const override { return true; }
			virtual std::chrono::microseconds cooldown() const override { return 60_s; }
			virtual bool requirements(const Actor* source) const override {
				return source->auraCount("hide-buff", source);
			}
		};

		_registerAction<Skill>();
	}

	{
		struct Skill : Action {
			Skill() : Action("sneak-attack") { }
			virtual int damage() const override { return 500; }
			virtual bool isOffGlobalCooldown() const override { return true; }
			virtual std::chrono::microseconds cooldown() const override { return 60_s; }
			virtual bool requirements(const Actor* source) const override {
				return source->auraCount("hide-buff", source);
			}
		};

		_registerAction<Skill>();
	}

	struct Ninjutsu : Aura {
		Ninjutsu() : Aura("ninjutsu") {}
		virtual std::chrono::microseconds duration() const override { return 20_s; }
	};

	struct Kassatsu : Aura {
		Kassatsu() : Aura("kassatsu-buff") {}
		virtual std::chrono::microseconds duration() const override { return 15_s; }
	};

	struct Hide : Aura {
		Hide() : Aura("hide-buff") {}
		virtual std::chrono::microseconds duration() const override { return 20_s; }
	};

	{
		struct Skill : Action {
			Skill() : Action("kassatsu") {}
			virtual bool isOffGlobalCooldown() const override { return true; }
			virtual std::chrono::microseconds cooldown() const override { return 120_s; }
			virtual void resolution(Actor* source, Actor* target, bool isCritical) const override {
				source->applyAura(&kassatsu, source);
			}
			Kassatsu kassatsu;
		};

		_registerAction<Skill>();
	}

	{
		struct Skill : Action {
			Skill() : Action("huton") {}
			virtual bool isOffGlobalCooldown() const override { return true; }
			virtual bool requirements(const Actor* source) const override {
				return !source->auraCount("ninjutsu", source);
			}
			virtual void resolution(Actor* source, Actor* target, bool isCritical) const override {
				source->applyAura(&ninjutsu, source);
				source->applyAura(&skillSpeedUp, source);
				source->dispelAura("kassatsu-buff", source);
			}
			Ninjutsu ninjutsu;
			SkillSpeedUp skillSpeedUp;
		};

		_registerAction<Skill>();
	}

	{
		struct Skill : Action {
			Skill() : Action("katon") {}
			virtual int damage() const override { return 180; }
			virtual bool isOffGlobalCooldown() const override { return true; }
			virtual bool requirements(const Actor* source) const override {
				return !source->auraCount("ninjutsu", source);
			}
			virtual void resolution(Actor* source, Actor* target, bool isCritical) const override {
				source->applyAura(&ninjutsu, source);
				source->dispelAura("kassatsu-buff", source);
			}
			Ninjutsu ninjutsu;
		};

		_registerAction<Skill>();
	}

	{
		struct Skill : Action {
			Skill() : Action("katon-crit") {}
			virtual int damage() const override { return 180; }
			virtual double criticalHitChance(const Actor* source, double base) const override { return 1.0; }
			virtual bool isOffGlobalCooldown() const override { return true; }
			virtual bool requirements(const Actor* source) const override {
				return source->auraCount("kassatsu-buff", source);
			}
			virtual void resolution(Actor* source, Actor* target, bool isCritical) const override {
				source->applyAura(&ninjutsu, source);
				source->dispelAura("kassatsu-buff", source);
			}
			Ninjutsu ninjutsu;
		};

		_registerAction<Skill>();
	}

	{
		struct Skill : Action {
			Skill() : Action("raiton") {}
			virtual int damage() const override { return 360; }
			virtual bool isOffGlobalCooldown() const override { return true; }
			virtual bool requirements(const Actor* source) const override {
				return !source->auraCount("ninjutsu", source);
			}
			virtual void resolution(Actor* source, Actor* target, bool isCritical) const override {
				source->applyAura(&ninjutsu, source);
				source->dispelAura("kassatsu-buff", source);
			}
			Ninjutsu ninjutsu;
		};

		_registerAction<Skill>();
	}

	{
		struct Skill : Action {
			Skill() : Action("raiton-crit") {}
			virtual int damage() const override { return 360; }
			virtual double criticalHitChance(const Actor* source, double base) const override { return 1.0; }
			virtual bool isOffGlobalCooldown() const override { return true; }
			virtual bool requirements(const Actor* source) const override {
				return source->auraCount("kassatsu-buff", source);
			}
			virtual void resolution(Actor* source, Actor* target, bool isCritical) const override {
				source->applyAura(&ninjutsu, source);
				source->dispelAura("kassatsu-buff", source);
			}
			Ninjutsu ninjutsu;
		};

		_registerAction<Skill>();
	}

	{
		struct Skill : Action {
			Skill() : Action("hyoton") {}
			virtual int damage() const override { return 140; }
			virtual bool isOffGlobalCooldown() const override { return true; }
			virtual bool requirements(const Actor* source) const override {
				return !source->auraCount("ninjutsu", source);
			}
			virtual void resolution(Actor* source, Actor* target, bool isCritical) const override {
				source->applyAura(&ninjutsu, source);
				source->dispelAura("kassatsu-buff", source);
			}
			Ninjutsu ninjutsu;
		};

		_registerAction<Skill>();
	}

	{
		struct Skill : Action {
			Skill() : Action("hyoton-crit") {}
			virtual int damage() const override { return 140; }
			virtual double criticalHitChance(const Actor* source, double base) const override { return 1.0; }
			virtual bool isOffGlobalCooldown() const override { return true; }
			virtual bool requirements(const Actor* source) const override {
				return source->auraCount("kassatsu-buff", source);
			}
			virtual void resolution(Actor* source, Actor* target, bool isCritical) const override {
				source->applyAura(&ninjutsu, source);
				source->dispelAura("kassatsu-buff", source);
			}
			Ninjutsu ninjutsu;
		};

		_registerAction<Skill>();
	}

	{
		struct DoT : Aura {
			DoT() : Aura("doton-dot") {}
			virtual std::chrono::microseconds duration() const override { return 24_s; }
			virtual int tickDamage() const override { return 30; }
		};

		struct Skill : Action {
			Skill() : Action("doton") {
				_targetAuras.push_back(new DoT());
			}
			virtual bool isOffGlobalCooldown() const override { return true; }
			virtual bool requirements(const Actor* source) const override {
				return !source->auraCount("ninjutsu", source);
			}
			virtual void resolution(Actor* source, Actor* target, bool isCritical) const override {
				source->applyAura(&ninjutsu, source);
				source->dispelAura("kassatsu-buff", source);
			}
			Ninjutsu ninjutsu;
		};

		_registerAction<Skill>();
	}

	{
		struct DoT : Aura {
			DoT() : Aura("doton-dot") {}
			virtual std::chrono::microseconds duration() const override { return 24_s; }
			virtual int tickDamage() const override { return 30; }
		};

		struct Skill : Action {
			Skill() : Action("doton-crit") {
				_targetAuras.push_back(new DoT());
			}
			virtual double criticalHitChance(const Actor* source, double base) const override { return 1.0; }
			virtual bool isOffGlobalCooldown() const override { return true; }
			virtual bool requirements(const Actor* source) const override {
				return source->auraCount("kassatsu-buff", source);
			}
			virtual void resolution(Actor* source, Actor* target, bool isCritical) const override {
				source->applyAura(&ninjutsu, source);
				source->dispelAura("kassatsu-buff", source);
			}
			Ninjutsu ninjutsu;
		};

		_registerAction<Skill>();
	}

	{
		struct Skill : Action {
			Skill() : Action("suiton") {}
			virtual int damage() const override { return 180; }
			virtual bool isOffGlobalCooldown() const override { return true; }
			virtual bool requirements(const Actor* source) const override {
				return !source->auraCount("ninjutsu", source);
			}
			virtual void resolution(Actor* source, Actor* target, bool isCritical) const override {
				source->applyAura(&ninjutsu, source);
				source->applyAura(&hide, source);
				source->dispelAura("kassatsu-buff", source);
			}
			Ninjutsu ninjutsu;
			Hide hide;
		};

		_registerAction<Skill>();
	}

	{
		struct Skill : Action {
			Skill() : Action("suiton-crit") {}
			virtual int damage() const override { return 180; }
			virtual double criticalHitChance(const Actor* source, double base) const override { return 1.0; }
			virtual bool isOffGlobalCooldown() const override { return true; }
			virtual bool requirements(const Actor* source) const override {
				return source->auraCount("kassatsu-buff", source);
			}
			virtual void resolution(Actor* source, Actor* target, bool isCritical) const override {
				source->applyAura(&ninjutsu, source);
				source->applyAura(&hide, source);
				source->dispelAura("kassatsu-buff", source);
			}
			Ninjutsu ninjutsu;
			Hide hide;
		};

		_registerAction<Skill>();
	}
}

void Ninja::prepareForBattle(Actor* actor) const {
	actor->applyAura(&skillSpeedUp, actor);
	actor->applyAura(&kissDamageBuff, actor);
}
int Ninja::maximumMP(const Actor* actor) const {
	return 0;
}

DamageType Ninja::_defaultDamageType() const {
	return DamageTypeSlashing;
}

std::chrono::microseconds Ninja::_baseGlobalCooldown(const Actor* actor) const {
	auto& stats = actor->stats();
	auto gcd = std::chrono::duration<double>(2.5 - (stats.skillSpeed - 341) * (0.01 / 10.5));
	return std::chrono::duration_cast<std::chrono::microseconds>(gcd);
}

// http://valk.dancing-mad.com/

double Ninja::_basePotencyMultiplier(const Actor* actor) const {
	auto& stats = actor->stats();
	return 0.01 * (stats.weaponPhysicalDamage * (stats.dexterity * 0.00389 + stats.determination * 0.0008 + 0.01035) + (stats.dexterity * 0.08034) + (stats.determination * 0.02622));
}

double Ninja::_baseAutoAttackDamage(const Actor* actor) const {
	auto& stats = actor->stats();
	return stats.weaponDelay / 3.0 * (stats.weaponPhysicalDamage * (stats.dexterity * 0.00408 + stats.determination * 0.00208 - 0.30991) + (stats.dexterity * 0.07149) + (stats.determination * 0.03443));
}

}
