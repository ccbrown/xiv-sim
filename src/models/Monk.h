#pragma once

#include "Base.h"

#include "../Aura.h"

namespace models {

class Monk : public Base {
	public:
		Monk();

		virtual void prepareForBattle(Actor* actor) const override;
		virtual int maximumMP(const Actor* actor) const override;

	protected:
		virtual DamageType _defaultDamageType() const override;

		virtual std::chrono::microseconds _baseGlobalCooldown(const Actor* actor) const override;
		virtual double _basePotencyMultiplier(const Actor* actor) const override;
		virtual double _baseAutoAttackDamage(const Actor* actor) const override;

		struct FistsOfFire : Aura {
			FistsOfFire() : Aura("fists-of-fire") {}
			virtual std::chrono::microseconds duration() const override { return std::chrono::microseconds::max(); }
			virtual double increasedDamage() const override { return 0.05; }
		};
		
		FistsOfFire fistsOfFire;
};

}