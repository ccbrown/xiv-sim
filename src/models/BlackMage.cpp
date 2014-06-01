#include "BlackMage.h"

#include "../Action.h"
#include "../Actor.h"
#include "../Aura.h"

#include <random>

namespace models {

BlackMage::BlackMage() : Base("black-mage") {
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
	
	struct Thundercloud : Aura {
		Thundercloud() : Aura("thundercloud") {}
		virtual std::chrono::microseconds duration() const override { return 12_s; }
	};
			
	{
		// thunder

		struct DoT : Aura {
			DoT() : Aura("thunder-dot") {}
			virtual std::chrono::microseconds duration() const override { return 18_s; }
			virtual int tickDamage() const override { return 35; }
			virtual void tick(Actor* actor, Actor* source, int count, bool isCritical) const override {
				std::uniform_real_distribution<double> distribution(0.0, 1.0);				
				if (distribution(actor->rng()) < 0.05) {
					source->applyAura(&thundercloud, source);
				}
			}
			Thundercloud thundercloud;
		};

		struct ThundercloudSpell : Action {
			ThundercloudSpell() : Action("thunder-thundercloud") {
				_targetAuras.push_back(new DoT());
			}
			virtual int damage() const override { return 30 + 12 / 3 * 40; }
			virtual void resolution(Actor* source, Actor* target) const override {
				source->dispelAura("thundercloud", source);
			}
		};

		_registerAction<ThundercloudSpell>();

		struct Spell : Action {
			Spell() : Action("thunder") {
				_targetAuras.push_back(new DoT());
			}
			virtual int damage() const override { return 30; }
			virtual int mpCost() const override { return 212; }
			virtual std::chrono::microseconds castTime() const override { return 2500_ms; }
			virtual const Action* replacement(const Actor* source, const Actor* target) const override {
				return source->auraCount("thundercloud", source) ? &thundercloudAction : nullptr;
			};

			ThundercloudSpell thundercloudAction;
		};
	
		_registerAction<Spell>();
	}

	{
		// thunder-ii

		struct DoT : Aura {
			DoT() : Aura("thunder-dot") {}
			virtual std::chrono::microseconds duration() const override { return 21_s; }
			virtual int tickDamage() const override { return 35; }
			virtual void tick(Actor* actor, Actor* source, int count, bool isCritical) const override {
				std::uniform_real_distribution<double> distribution(0.0, 1.0);				
				if (distribution(actor->rng()) < 0.05) {
					source->applyAura(&thundercloud, source);
				}
			}
			Thundercloud thundercloud;
		};

		struct ThundercloudSpell : Action {
			ThundercloudSpell() : Action("thunder-ii-thundercloud") {
				_targetAuras.push_back(new DoT());
			}
			virtual int damage() const override { return 50 + 15 / 3 * 40; }
			virtual void resolution(Actor* source, Actor* target) const override {
				source->dispelAura("thundercloud", source);
			}
		};

		_registerAction<ThundercloudSpell>();

		struct Spell : Action {
			Spell() : Action("thunder-ii") {
				_targetAuras.push_back(new DoT());
			}
			virtual int damage() const override { return 50; }
			virtual int mpCost() const override { return 319; }
			virtual std::chrono::microseconds castTime() const override { return 3_s; }
			virtual const Action* replacement(const Actor* source, const Actor* target) const override {
				return source->auraCount("thundercloud", source) ? &thundercloudAction : nullptr;
			};

			ThundercloudSpell thundercloudAction;
		};
	
		_registerAction<Spell>();
	}

	{
		// thunder-iii

		struct DoT : Aura {
			DoT() : Aura("thunder-dot") {}
			virtual std::chrono::microseconds duration() const override { return 24_s; }
			virtual int tickDamage() const override { return 35; }
			virtual void tick(Actor* actor, Actor* source, int count, bool isCritical) const override {
				std::uniform_real_distribution<double> distribution(0.0, 1.0);				
				if (distribution(actor->rng()) < 0.05) {
					source->applyAura(&thundercloud, source);
				}
			}
			Thundercloud thundercloud;
		};

		struct ThundercloudSpell : Action {
			ThundercloudSpell() : Action("thunder-iii-thundercloud") {
				_targetAuras.push_back(new DoT());
			}
			virtual int damage() const override { return 60 + 18 / 3 * 40; }
			virtual void resolution(Actor* source, Actor* target) const override {
				source->dispelAura("thundercloud", source);
			}
		};

		_registerAction<ThundercloudSpell>();

		struct Spell : Action {
			Spell() : Action("thunder-iii") {
				_targetAuras.push_back(new DoT());
			}
			virtual int damage() const override { return 60; }
			virtual int mpCost() const override { return 425; }
			virtual std::chrono::microseconds castTime() const override { return 3500_ms; }
			virtual const Action* replacement(const Actor* source, const Actor* target) const override {
				return source->auraCount("thundercloud", source) ? &thundercloudAction : nullptr;
			};

			ThundercloudSpell thundercloudAction;
		};
	
		_registerAction<Spell>();
	}

	struct AstralFire : Aura {
		AstralFire() : Aura("astral-fire") {}
		virtual int maximumCount() const override { return 3; }
		virtual std::chrono::microseconds duration() const override { return 10_s; }
		virtual double mpRegenMultiplier() const override { return 0.0; }
	};

	struct UmbralIce : Aura {
		UmbralIce() : Aura("umbral-ice") {}
		virtual int maximumCount() const override { return 3; }
		virtual std::chrono::microseconds duration() const override { return 10_s; }
		virtual void tick(Actor* actor, Actor* source, int count, bool isCritical) const override {
			actor->setMP(actor->mp() + actor->maximumMP() * (0.15 + count * 0.15));
		}
	};

	{
		struct Spell : Action {
			Spell() : Action("blizzard") {}
			virtual int damage(const Actor* source, const Actor* target) const override { return IceSpellDamage(source, 150); }
			virtual int mpCost(const Actor* source) const override { return IceSpellManaCost(source, 106); }
			virtual std::chrono::microseconds castTime(const Actor* source) const override {
				return (source->auraCount("astral-fire", source) == 3) ? 1250_ms : 2500_ms;
			}
			virtual void resolution(Actor* source, Actor* target) const override {
				if (!source->dispelAura("astral-fire", source, 3)) {
					source->applyAura(&umbralIce, source);
				}
			}
			UmbralIce umbralIce;
		};
	
		_registerAction<Spell>();
	}

	{
		struct Firestarter : Aura {
			Firestarter() : Aura("firestarter") {}
			virtual std::chrono::microseconds duration() const override { return 12_s; }
		};

		struct PossibleFirestarter : Aura {
			PossibleFirestarter() : Aura("possible-firestarter") {}
			virtual bool isHidden() const override { return true; }
			virtual std::chrono::microseconds duration() const override { return 300_ms; }
			virtual void afterEffect(Actor* actor, Actor* source, int count) const override {
				std::uniform_real_distribution<double> distribution(0.0, 1.0);
				if (distribution(source->rng()) < 0.40) {
					source->applyAura(&firestarter, source);
				}
			}
			
			Firestarter firestarter;
		};

		struct Spell : Action {
			Spell() : Action("fire") {}
			virtual int damage(const Actor* source, const Actor* target) const override { return FireSpellDamage(source, 150); }
			virtual int mpCost(const Actor* source) const override { return FireSpellManaCost(source, 319); }
			virtual std::chrono::microseconds castTime(const Actor* source) const override {
				return (source->auraCount("umbral-ice", source) == 3) ? 1250_ms : 2500_ms;
			}
			virtual void resolution(Actor* source, Actor* target) const override {
				if (!source->dispelAura("umbral-ice", source, 3)) {
					source->applyAura(&astralFire, source);
				}
				source->applyAura(&possibleFirestarter, source);
			}
			AstralFire astralFire;
			PossibleFirestarter possibleFirestarter;
		};
	
		_registerAction<Spell>();
	}

	{
		struct FirestarterSpell : Action {
			FirestarterSpell() : Action("fire-iii-firestarter") {}
			virtual int damage(const Actor* source, const Actor* target) const override { return FireSpellDamage(source, 220); }
			virtual void resolution(Actor* source, Actor* target) const override {
				source->dispelAura("possible-firestarter", source);
				source->dispelAura("firestarter", source);
				source->dispelAura("umbral-ice", source, 3);
				source->applyAura(&astralFire, source, 3);
			}
			AstralFire astralFire;
		};
	
		_registerAction<FirestarterSpell>();
	
		struct Spell : Action {
			Spell() : Action("fire-iii") {}
			virtual int damage(const Actor* source, const Actor* target) const override { return FireSpellDamage(source, 220); }
			virtual int mpCost(const Actor* source) const override { return FireSpellManaCost(source, 532); }
			virtual std::chrono::microseconds castTime(const Actor* source) const override {
				return (source->auraCount("umbral-ice", source) == 3) ? 1750_ms : 3500_ms;
			}
			virtual void resolution(Actor* source, Actor* target) const override {
				source->dispelAura("umbral-ice", source, 3);
				source->applyAura(&astralFire, source, 3);
			}
			virtual const Action* replacement(const Actor* source, const Actor* target) const override {
				return source->auraCount("firestarter", source) ? &firestarterAction : nullptr;
			};
			AstralFire astralFire;
			FirestarterSpell firestarterAction;
		};
	
		_registerAction<Spell>();
	}

	{
		struct Spell : Action {
			Spell() : Action("blizzard-iii") {}
			virtual int damage(const Actor* source, const Actor* target) const override { return IceSpellDamage(source, 220); }
			virtual int mpCost(const Actor* source) const override { return IceSpellManaCost(source, 319); }
			virtual std::chrono::microseconds castTime(const Actor* source) const override {
				return (source->auraCount("astral-fire", source) == 3) ? 1750_ms : 3500_ms;
			}
			virtual void resolution(Actor* source, Actor* target) const override {
				source->dispelAura("astral-fire", source, 3);
				source->applyAura(&umbralIce, source, 3);
			}
			UmbralIce umbralIce;
		};
	
		_registerAction<Spell>();
	}

	{
		struct Spell : Action {
			Spell() : Action("transpose") {}
			virtual std::chrono::microseconds cooldown() const override { return 12_s; }
			virtual void resolution(Actor* source, Actor* target) const override {
				if (source->dispelAura("astral-fire", source, 3)) {
					source->applyAura(&umbralIce, source);
				} else if (source->dispelAura("umbral-ice", source, 3)) {
					source->applyAura(&astralFire, source);
				}
			}
			UmbralIce umbralIce;
			AstralFire astralFire;
		};
	
		_registerAction<Spell>();
	}

	{
		struct Spell : Action {
			Spell() : Action("convert") {}
			virtual std::chrono::microseconds cooldown() const override { return 180_s; }
			virtual void resolution(Actor* source, Actor* target) const override {
				source->setMP(source->mp() + source->maximumMP() * 0.30);
			}
		};
	
		_registerAction<Spell>();
	}

	{
		struct Spell : Action {
			Spell() : Action("flare") {}
			virtual int damage(const Actor* source, const Actor* target) const override { return FireSpellDamage(source, 260); }
			virtual int mpCost(const Actor* source) const override { return source->mp(); }
			virtual std::chrono::microseconds castTime() const override { return 4_s; }
			virtual void resolution(Actor* source, Actor* target) const override {
				source->dispelAura("umbral-ice", source, 3);
				source->applyAura(&astralFire, source, 3);
			}
			AstralFire astralFire;
		};
	
		_registerAction<Spell>();
	}
}

int BlackMage::maximumMP(const Actor* actor) const {
	// TODO: double check this. was copied / pasted from summoner and it's extremely important that this is accurate
	return 3012 + (actor->stats().piety - 214) * 7.25;
}

DamageType BlackMage::_defaultDamageType() const {
	return DamageTypeMagic;
}

std::chrono::microseconds BlackMage::_baseGlobalCooldown(const Actor* actor) const {
	auto& stats = actor->stats();
	auto gcd = std::chrono::duration<double>(2.5 - (stats.spellSpeed - 341) * (0.01 / 10.5));
	return std::chrono::duration_cast<std::chrono::microseconds>(gcd);
}

// http://www.reddit.com/r/ffxiv/comments/1nxw7m/caster_damage_formula_testingstat_weights/

double BlackMage::_basePotencyMultiplier(const Actor* actor) const {
	auto& stats = actor->stats();
	return 0.01 * (stats.weaponMagicDamage * stats.intelligence * 0.00587517 + stats.determination * 0.074377 + stats.intelligence * 0.077076);
}

// TODO: this number may be wrong (was copied / pasted from monk)

double BlackMage::_baseAutoAttackDamage(const Actor* actor) const {
	auto& stats = actor->stats();
	return stats.weaponDelay / 3.0 * (stats.weaponPhysicalDamage * (stats.strength * 0.00408 + stats.determination * 0.00208 - 0.30991) + (stats.strength * 0.07149) + (stats.determination * 0.03443));
}

int BlackMage::IceSpellManaCost(const Actor* actor, int base) {
	if (auto count = actor->auraCount("astral-fire", actor)) {
		return count >= 2 ? (base / 4) : (base / 2);
	}
	return base;
}

int BlackMage::IceSpellDamage(const Actor* actor, int base) {
	if (auto count = actor->auraCount("astral-fire", actor)) {
		return base * (1.0 - 0.1 * count);
	}
	return base;
}

int BlackMage::FireSpellManaCost(const Actor* actor, int base) {
	if (auto count = actor->auraCount("umbral-ice", actor)) {
		return count >= 2 ? (base / 4) : (base / 2);
	} else if (actor->auraCount("astral-fire", actor)) {
		return base * 2;
	}
	return base;
}

int BlackMage::FireSpellDamage(const Actor* actor, int base) {
	if (auto count = actor->auraCount("umbral-ice", actor)) {
		return base * (1.0 - 0.1 * count);
	} else if (auto count = actor->auraCount("astral-fire", actor)) {
		return base * (1.0 + 0.25 * count);
	}
	return base;
}

}
