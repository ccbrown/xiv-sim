#include "Warrior.h"

#include "../Action.h"
#include "../Actor.h"
#include "../Aura.h"

namespace models {

Warrior::Warrior() : Base("warrior") {
	struct Wrath : Aura {
		Wrath() : Aura("wrath") {}
		virtual std::chrono::microseconds duration() const override { return 30_s; }
		virtual double additionalCriticalHitChance() const override { return 0.02; }
		virtual int maximumCount() const override { return 5; }
	};

// XXX: not quite right... it really just needs to temporarily cancel Defiance debuff
	struct DefianceOverride : Aura {
		DefianceOverride() : Aura("defiance-override") {}
		virtual bool isHidden() const override { return true; }
		virtual std::chrono::microseconds duration() const override { return 250_ms; }
		virtual void transformIncomingDamage(Damage* damage) const override {
			damage->amount *= 1.0 / 0.66;
		}
	};

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
			struct DoT : Aura {
				DoT() : Aura("fracture-dot") {}
				virtual std::chrono::microseconds duration() const override { return 30_s; }
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

	{
		struct Skill : Action {
			Skill() : Action("heavy-swing") {}
			virtual int damage() const override { return 150; }
			virtual int tpCost() const override { return 70; }
		};

		_registerAction<Skill>();
	}

	{
		struct Skill : Action {
			Skill() : Action("skull-sunder-combo") {}
			virtual int damage() const override { return 200; }
			virtual int tpCost() const override { return 60; }
			virtual bool requirements(const Actor* source) const override {
				return source->comboAction() && source->comboAction()->identifier() == "heavy-swing";
			}
			virtual void resolution(Actor* source, Actor* target, bool isCritical) const override {
				source->dispelAura("heavy-swing", source);
				if (source->auraCount("defiance", source))
					source->applyAura(&wrath, source);
			}
			Wrath wrath;
		};

		_registerAction<Skill>();
	}

	{
		struct Skill : Action {
			Skill() : Action("brutal-swing") {}
			virtual int damage() const override { return 100; }
			virtual bool isOffGlobalCooldown() const override { return true; }
			virtual std::chrono::microseconds cooldown() const override { return 20_s; }
		};

		_registerAction<Skill>();
	}

	{
		struct Skill : Action {
			Skill() : Action("overpower") {}
			virtual int damage() const override { return 120; }
			virtual int tpCost() const override { return 130; }
		};

		_registerAction<Skill>();
	}

	{
		struct Skill : Action {
			Skill() : Action("tomahawk") {}
			virtual int damage() const override { return 130; }
			virtual int tpCost() const override { return 120; }
			// Tomahawk is a ranged attack, so only valid for opener
			virtual std::chrono::microseconds cooldown() const override { return std::chrono::microseconds::max(); }
		};

		_registerAction<Skill>();
	}

	{
		struct Skill : Action {
			struct Debuff : Aura {
				Debuff() : Aura("maim-debuff") {}
				virtual std::chrono::microseconds duration() const override { return 24_s; }
				virtual void transformIncomingDamage(Damage* damage) const override {
					damage->amount *= 1.0 / 0.8;
				}
			};

			Skill() : Action("maim-combo") {
				_targetAuras.push_back(new Debuff());
			}
			virtual int damage() const override { return 190; }
			virtual bool requirements(const Actor* source) const override {
				return source->comboAction() && source->comboAction()->identifier() == "heavy-swing";
			}
			virtual void resolution(Actor* source, Actor* target, bool isCritical) const override {
				if (source->auraCount("defiance", source))
					source->applyAura(&wrath, source);
			}
			Wrath wrath;
		};

		_registerAction<Skill>();
	}

	{
		struct Skill : Action {
			struct Buff : Aura {
				Buff() : Aura("berserk") {
					_statsMultiplier.strength = 1.5;
				}
				virtual std::chrono::microseconds duration() const override { return 20_s; }
			};

			Skill() : Action("berserk") {
				_sourceAuras.push_back(new Buff());
			}
			virtual bool isOffGlobalCooldown() const override { return true; }
			virtual std::chrono::microseconds cooldown() const override { return 90_s; }
			virtual void resolution(Actor* source, Actor* target, bool isCritical) const override {
				if (source->auraCount("defiance", source))
					source->applyAura(&wrath, source);
			}
			Wrath wrath;
		};

		_registerAction<Skill>();
	}

	{
		struct Skill : Action {
			Skill() : Action("defiance") {
				_sourceAuras.push_back(new Defiance());
			}
			virtual std::chrono::microseconds cooldown() const override { return 10_s; }
		};

		_registerAction<Skill>();
	}

	{
		struct Skill : Action {
			Skill() : Action("butchers-block-combo") {}
			virtual int damage() const override { return 280; }
			virtual int tpCost() const override { return 60; }
			virtual bool requirements(const Actor* source) const override {
				return source->comboAction() && source->comboAction()->identifier() == "skull-sunder-combo";
			}
			virtual void resolution(Actor* source, Actor* target, bool isCritical) const override {
				source->dispelAura("skull-sunder-combo", source);
				if (source->auraCount("defiance", source))
					source->applyAura(&wrath, source);
			}
			Wrath wrath;
		};

		_registerAction<Skill>();
	}

	{
		struct Skill : Action {
			Skill() : Action("inner-beast") {
// XXX: needs to ignore defiance 25% damage penalty
				_targetAuras.push_back(new DefianceOverride());
			}
			virtual int damage() const override { return 300; }
			virtual bool requirements(const Actor* source) const override {
				return source->auraCount("wrath", source) == 5;
			}
			virtual void resolution(Actor* source, Actor* target, bool isCritical) const override {
				source->dispelAura("wrath", source, 5);
			}
		};

		_registerAction<Skill>();
	}

	{
		struct Skill : Action {
			Skill() : Action("storms-path-combo") {}
			virtual int damage() const override { return 250; }
			virtual int tpCost() const override { return 60; }
			virtual bool requirements(const Actor* source) const override {
				return source->comboAction() && source->comboAction()->identifier() == "maim-combo";
			}
			virtual void resolution(Actor* source, Actor* target, bool isCritical) const override {
				source->dispelAura("maim-combo", source);
				if (source->auraCount("defiance", source))
					source->applyAura(&wrath, source);
			}
			Wrath wrath;
		};

		_registerAction<Skill>();
	}

	{
		struct Skill : Action {
			Skill() : Action("unchained") {
				_sourceAuras.push_back(new Unchained());
			}
			virtual bool requirements(const Actor* source) const override {
				return source->auraCount("wrath", source) == 5;
			}
			virtual void resolution(Actor* source, Actor* target, bool isCritical) const override {
				source->dispelAura("wrath", source, 5);
			}
			virtual bool isOffGlobalCooldown() const override { return true; }
			virtual std::chrono::microseconds cooldown() const override { return 120_s; }
		};

		_registerAction<Skill>();
	}

	{
		struct Skill : Action {
			Skill() : Action("steel-cyclone") {
// XXX: needs to ignore defiance 25% damage penalty
				_targetAuras.push_back(new DefianceOverride());
			}
			virtual int damage() const override { return 200; }
			virtual bool requirements(const Actor* source) const override {
				return source->auraCount("wrath", source) == 5;
			}
			virtual void resolution(Actor* source, Actor* target, bool isCritical) const override {
				source->dispelAura("wrath", source, 5);
			}
		};

		_registerAction<Skill>();
	}

	{
		struct Skill : Action {
			Skill() : Action("infuriate") {}
			virtual bool requirements(const Actor* source) const override {
				return source->auraCount("defiance", source);
			}
			virtual bool isOffGlobalCooldown() const override { return true; }
			virtual std::chrono::microseconds cooldown() const override { return 60_s; }
			virtual void resolution(Actor* source, Actor* target, bool isCritical) const override {
				source->applyAura(&wrath, source, 5);
			}
			Wrath wrath;
		};

		_registerAction<Skill>();
	}

	{
		struct Skill : Action {
			struct Debuff : Aura {
				Debuff() : Aura("storms-eye-debuff") {}
				virtual std::chrono::microseconds duration() const override { return 20_s; }
				virtual void transformIncomingDamage(Damage* damage) const override {
					if (damage->type == DamageTypeSlashing) {
						damage->amount *= 1.0 / 0.9;
					}
				}
			};

			Skill() : Action("storms-eye-combo") {
				_targetAuras.push_back(new Debuff());
			}
			virtual int damage() const override { return 270; }
			virtual int tpCost() const override { return 60; }
			virtual bool requirements(const Actor* source) const override {
				return source->comboAction() && source->comboAction()->identifier() == "maim-combo";
			}
			virtual void resolution(Actor* source, Actor* target, bool isCritical) const override {
				source->dispelAura("maim-combo", source);
				if (source->auraCount("defiance", source))
					source->applyAura(&wrath, source);
			}
			Wrath wrath;
		};

		_registerAction<Skill>();
	}
}

void Warrior::prepareForBattle(Actor* actor) const {
	//actor->applyAura(&defiance, actor);
}
int Warrior::maximumMP(const Actor* actor) const {
	return 0;
}

DamageType Warrior::_defaultDamageType() const {
	return DamageTypeSlashing;
}

std::chrono::microseconds Warrior::_baseGlobalCooldown(const Actor* actor) const {
	auto& stats = actor->stats();
	auto gcd = std::chrono::duration<double>(2.5 - (stats.skillSpeed - 341) * (0.01 / 10.5));
	return std::chrono::duration_cast<std::chrono::microseconds>(gcd);
}

// http://valk.dancing-mad.com/

double Warrior::_basePotencyMultiplier(const Actor* actor) const {
	auto& stats = actor->stats();
	return 0.01 * (stats.weaponPhysicalDamage * (stats.strength * 0.00389 + stats.determination * 0.0008 + 0.01035) + (stats.strength * 0.08034) + (stats.determination * 0.02622));
}

double Warrior::_baseAutoAttackDamage(const Actor* actor) const {
	auto& stats = actor->stats();
	return stats.weaponDelay / 3.0 * (stats.weaponPhysicalDamage * (stats.strength * 0.00408 + stats.determination * 0.00208 - 0.30991) + (stats.strength * 0.07149) + (stats.determination * 0.03443));
}

}
