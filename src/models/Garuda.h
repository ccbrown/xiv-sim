#pragma once

#include "Base.h"

namespace models {

class Garuda : public Base {
	public:
		Garuda();

		virtual std::chrono::microseconds globalCooldown(const Actor* actor) const override;
		virtual int maximumMP(const Actor* actor) const override { return 0; }
		virtual std::chrono::microseconds autoAttackInterval(const Actor* actor) const override;
		virtual std::chrono::microseconds castTime(const Action* action, const Actor* actor) const override;

	protected:
		virtual DamageType _defaultDamageType() const override;

		virtual std::chrono::microseconds _baseGlobalCooldown(const Actor* actor) const override;
		virtual double _basePotencyMultiplier(const Actor* actor) const override;
		virtual double _baseAutoAttackDamage(const Actor* actor) const override { return 0; }
};

}