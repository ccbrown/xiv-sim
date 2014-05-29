#include "BlackMage.h"

#include "../Action.h"
#include "../Actor.h"
#include "../Aura.h"

#include <memory>
#include <random>

namespace models {

BlackMage::BlackMage() {
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
			DoT() : Aura("thunder-dot"), thunderCloud(new Thundercloud()) {}
			virtual std::chrono::microseconds duration() const override { return 12_s; }
			virtual int tickDamage() const override { return 40; }
			virtual void tick(Actor* actor, Actor* source, int count) const override {
				std::uniform_real_distribution<double> distribution(0.0, 1.0);				
				if (distribution(actor->rng()) < 0.05) {
					source->applyAura(thunderCloud.get(), source);
				}
			}
			
			std::unique_ptr<const Aura> thunderCloud;
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
			Spell() : Action("thunder"), thundercloudAction(new ThundercloudSpell()) {
				_targetAuras.push_back(new DoT());
			}
			virtual int damage() const override { return 30; }
			virtual int mpCost() const override { return 212; }
			virtual std::chrono::microseconds castTime() const override { return 2500_ms; }
			virtual const Action* replacement(const Actor* source, const Actor* target) const override {
				return source->auraCount("thundercloud", source) ? thundercloudAction.get() : nullptr;
			};

			std::unique_ptr<const Action> thundercloudAction;
		};
	
		_registerAction<Spell>();
	}

	{
		// thunder-ii

		struct DoT : Aura {
			DoT() : Aura("thunder-dot"), thunderCloud(new Thundercloud()) {}
			virtual std::chrono::microseconds duration() const override { return 15_s; }
			virtual int tickDamage() const override { return 40; }
			virtual void tick(Actor* actor, Actor* source, int count) const override {
				std::uniform_real_distribution<double> distribution(0.0, 1.0);				
				if (distribution(actor->rng()) < 0.05) {
					source->applyAura(thunderCloud.get(), source);
				}
			}
			
			std::unique_ptr<const Aura> thunderCloud;
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
			Spell() : Action("thunder-ii"), thundercloudAction(new ThundercloudSpell()) {
				_targetAuras.push_back(new DoT());
			}
			virtual int damage() const override { return 50; }
			virtual int mpCost() const override { return 319; }
			virtual std::chrono::microseconds castTime() const override { return 3_s; }
			virtual const Action* replacement(const Actor* source, const Actor* target) const override {
				return source->auraCount("thundercloud", source) ? thundercloudAction.get() : nullptr;
			};

			std::unique_ptr<const Action> thundercloudAction;
		};
	
		_registerAction<Spell>();
	}

	{
		// thunder-iii

		struct DoT : Aura {
			DoT() : Aura("thunder-dot"), thunderCloud(new Thundercloud()) {}
			virtual std::chrono::microseconds duration() const override { return 18_s; }
			virtual int tickDamage() const override { return 40; }
			virtual void tick(Actor* actor, Actor* source, int count) const override {
				std::uniform_real_distribution<double> distribution(0.0, 1.0);				
				if (distribution(actor->rng()) < 0.05) {
					source->applyAura(thunderCloud.get(), source);
				}
			}
			
			std::unique_ptr<const Aura> thunderCloud;
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
			Spell() : Action("thunder-iii"), thundercloudAction(new ThundercloudSpell()) {
				_targetAuras.push_back(new DoT());
			}
			virtual int damage() const override { return 60; }
			virtual int mpCost() const override { return 425; }
			virtual std::chrono::microseconds castTime() const override { return 3500_ms; }
			virtual const Action* replacement(const Actor* source, const Actor* target) const override {
				return source->auraCount("thundercloud", source) ? thundercloudAction.get() : nullptr;
			};

			std::unique_ptr<const Action> thundercloudAction;
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
		virtual void tick(Actor* actor, Actor* source, int count) const override {
			actor->setMP(actor->mp() + actor->maximumMP() * (0.15 + count * 0.15));
		}
	};

	{
		struct Spell : Action {
			Spell() : Action("blizzard"), elementalAura(new UmbralIce()) {}
			virtual int damage(const Actor* source, const Actor* target) const override { return IceSpellDamage(source, 150); }
			virtual int mpCost(const Actor* source) const override { return IceSpellManaCost(source, 106); }
			virtual std::chrono::microseconds castTime() const override { return 2500_ms; }
			virtual void resolution(Actor* source, Actor* target) const override {
				if (!source->dispelAura("astral-fire", source, 3)) {
					source->applyAura(elementalAura.get(), source);
				}
			}
			std::unique_ptr<const Aura> elementalAura;
		};
	
		_registerAction<Spell>();
	}

	{
		struct Firestarter : Aura {
			Firestarter() : Aura("firestarter") {}
			virtual std::chrono::microseconds duration() const override { return 12_s; }
		};

		// this is a bit of a hack. we currently don't simulate latency or travel times for spells, but
		// for firestarter that's actually especially significant. emulate it by waiting a bit before 
		// applying firestarter
		struct IncomingFirestarter : Aura {
			IncomingFirestarter() : Aura("incoming-firestarter"), firestarter(new Firestarter()) {}
			virtual bool isHidden() const override { return true; }
			virtual std::chrono::microseconds duration() const override { return 200_ms; }
			virtual void expiration(Actor* actor, Actor* source, int count) const override {
				actor->applyAura(firestarter.get(), source, count);
			}
			std::unique_ptr<const Aura> firestarter;
		};

		struct Spell : Action {
			Spell() : Action("fire"), elementalAura(new AstralFire()), incomingFirestarter(new IncomingFirestarter()) {}
			virtual int damage(const Actor* source, const Actor* target) const override { return FireSpellDamage(source, 150); }
			virtual int mpCost(const Actor* source) const override { return FireSpellManaCost(source, 319); }
			virtual std::chrono::microseconds castTime(const Actor* source) const override {
				return (source->auraCount("umbral-ice", source) == 3) ? 1_s : 2500_ms;
			}
			virtual void resolution(Actor* source, Actor* target) const override {
				if (!source->dispelAura("umbral-ice", source, 3)) {
					source->applyAura(elementalAura.get(), source);
				}
				std::uniform_real_distribution<double> distribution(0.0, 1.0);				
				if (distribution(source->rng()) < 0.40) {
					source->applyAura(incomingFirestarter.get(), source);
				}
			}
			std::unique_ptr<const Aura> elementalAura;
			std::unique_ptr<const Aura> incomingFirestarter;
		};
	
		_registerAction<Spell>();
	}

	{
		struct FirestarterSpell : Action {
			FirestarterSpell() : Action("fire-iii-firestarter"), elementalAura(new AstralFire()) {}
			virtual int damage(const Actor* source, const Actor* target) const override { return FireSpellDamage(source, 220); }
			virtual void resolution(Actor* source, Actor* target) const override {
				source->dispelAura("firestarter", source);
				source->dispelAura("umbral-ice", source, 3);
				source->applyAura(elementalAura.get(), source, 3);
			}
			std::unique_ptr<const Aura> elementalAura;
		};
	
		_registerAction<FirestarterSpell>();
	
		struct Spell : Action {
			Spell() : Action("fire-iii"), elementalAura(new AstralFire()), firestarterAction(new FirestarterSpell()) {}
			virtual int damage(const Actor* source, const Actor* target) const override { return FireSpellDamage(source, 220); }
			virtual int mpCost(const Actor* source) const override { return FireSpellManaCost(source, 532); }
			virtual std::chrono::microseconds castTime() const override { return 3500_ms; }
			virtual void resolution(Actor* source, Actor* target) const override {
				source->dispelAura("umbral-ice", source, 3);
				source->applyAura(elementalAura.get(), source, 3);
			}
			virtual const Action* replacement(const Actor* source, const Actor* target) const override {
				return source->auraCount("firestarter", source) ? firestarterAction.get() : nullptr;
			};
			std::unique_ptr<const Aura> elementalAura;
			std::unique_ptr<const Action> firestarterAction;
		};
	
		_registerAction<Spell>();
	}

	{
		struct Spell : Action {
			Spell() : Action("blizzard-iii"), elementalAura(new UmbralIce()) {}
			virtual int damage(const Actor* source, const Actor* target) const override { return IceSpellDamage(source, 220); }
			virtual int mpCost(const Actor* source) const override { return IceSpellManaCost(source, 319); }
			virtual std::chrono::microseconds castTime() const override { return 3500_ms; }
			virtual void resolution(Actor* source, Actor* target) const override {
				source->dispelAura("astral-fire", source, 3);
				source->applyAura(elementalAura.get(), source, 3);
			}
			std::unique_ptr<const Aura> elementalAura;
		};
	
		_registerAction<Spell>();
	}

	{
		struct Spell : Action {
			Spell() : Action("transpose"), umbralIce(new UmbralIce()), astralFire(new AstralFire()) {}
			virtual std::chrono::microseconds cooldown() const override { return 12_s; }
			virtual void resolution(Actor* source, Actor* target) const override {
				if (source->dispelAura("astral-fire", source, 3)) {
					source->applyAura(umbralIce.get(), source);
				} else if (source->dispelAura("umbral-ice", source, 3)) {
					source->applyAura(astralFire.get(), source);
				}
			}
			std::unique_ptr<const Aura> umbralIce;
			std::unique_ptr<const Aura> astralFire;
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
			Spell() : Action("flare"), elementalAura(new AstralFire()) {}
			virtual int damage(const Actor* source, const Actor* target) const override { return FireSpellDamage(source, 260); }
			virtual int mpCost(const Actor* source) const override { return source->mp(); }
			virtual std::chrono::microseconds castTime() const override { return 4_s; }
			virtual void resolution(Actor* source, Actor* target) const override {
				source->dispelAura("umbral-ice", source, 3);
				source->applyAura(elementalAura.get(), source, 3);
			}
			std::unique_ptr<const Aura> elementalAura;
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
	return 0.01 * (stats.weaponDamage * stats.intelligence * 0.00587517 + stats.determination * 0.074377 + stats.intelligence * 0.077076);
}

// TODO: this number is probably totally wrong (was copied / pasted from monk)

double BlackMage::_baseAutoAttackDamage(const Actor* actor) const {
	auto& stats = actor->stats();
	return stats.weaponDelay / 3.0 * (stats.weaponDamage * (stats.intelligence * 0.00408 + stats.determination * 0.00208 - 0.30991) + (stats.intelligence * 0.07149) + (stats.determination * 0.03443));
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
