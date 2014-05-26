#pragma once

#include "Base.h"

namespace models {

class Summoner : public Base {
	public:
		Summoner();

		virtual std::chrono::microseconds globalCooldown(const Actor* actor) const override;
		virtual int maximumMP(const Actor* actor) const override;

	protected:
		virtual DamageType _defaultDamageType() const override;
		virtual double _basePotencyMultiplier(const Actor* actor) const override;
		virtual double _baseAutoAttackDamage(const Actor* actor) const override;
};

}