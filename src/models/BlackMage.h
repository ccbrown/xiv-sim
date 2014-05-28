#pragma once

#include "Base.h"

namespace models {

class BlackMage : public Base {
	public:
		BlackMage();

		virtual int maximumMP(const Actor* actor) const override;

	protected:
		virtual DamageType _defaultDamageType() const override;

		virtual std::chrono::microseconds _baseGlobalCooldown(const Actor* actor) const override;
		virtual double _basePotencyMultiplier(const Actor* actor) const override;
		virtual double _baseAutoAttackDamage(const Actor* actor) const override;
		
		static int IceSpellManaCost(const Actor* actor, int base);
		static int IceSpellDamage(const Actor* actor, int base);
		static int FireSpellManaCost(const Actor* actor, int base);
		static int FireSpellDamage(const Actor* actor, int base);
};

}