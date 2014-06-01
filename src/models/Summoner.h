#pragma once

#include "Base.h"

#include "../Aura.h"

namespace models {

class Summoner : public Base {
	public:
		Summoner();

		virtual void prepareForBattle(Actor* actor) const override;
		virtual int maximumMP(const Actor* actor) const override;

	protected:
		virtual DamageType _defaultDamageType() const override;

		virtual std::chrono::microseconds _baseGlobalCooldown(const Actor* actor) const override;
		virtual double _basePotencyMultiplier(const Actor* actor) const override;
		virtual double _baseAutoAttackDamage(const Actor* actor) const override;

		struct Aetherflow : Aura {
			Aetherflow() : Aura("aetherflow") {}
			virtual int maximumCount() const override { return 3; }
			virtual std::chrono::microseconds duration() const override { return std::chrono::microseconds::max(); }
		};
		
		Aetherflow aetherflow;
};

}