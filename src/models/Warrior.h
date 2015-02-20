#pragma once

#include "Base.h"

#include "../Aura.h"

namespace models {

class Warrior : public Base {
	public:
		Warrior();

		virtual void prepareForBattle(Actor* actor) const override;
		virtual int maximumMP(const Actor* actor) const override;

	protected:
		virtual DamageType _defaultDamageType() const override;

		virtual std::chrono::microseconds _baseGlobalCooldown(const Actor* actor) const override;
		virtual double _basePotencyMultiplier(const Actor* actor) const override;
		virtual double _baseAutoAttackDamage(const Actor* actor) const override;

		struct Defiance : Aura {
			Defiance() : Aura("defiance") {}
			virtual std::chrono::microseconds duration() const override { return std::chrono::microseconds::max(); }
			virtual double increasedDamage() const override { return -0.25; }
		};
		Defiance defiance;

		struct Unchained : Aura {
			Unchained() : Aura("unchained") {}
			virtual std::chrono::microseconds duration() const override { return 20_s; }
// XXX: not quite right... it really just needs to temporarily cancel Defiance debuff
			virtual double increasedDamage() const override { return 0.33; }
		};
		Unchained unchained;
};

}
