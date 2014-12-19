#pragma once

#include "Base.h"

#include "../Aura.h"

namespace models {

class Ninja : public Base {
	public:
		Ninja();

		virtual void prepareForBattle(Actor* actor) const override;
		virtual int maximumMP(const Actor* actor) const override;

	protected:
		virtual DamageType _defaultDamageType() const override;

		virtual std::chrono::microseconds _baseGlobalCooldown(const Actor* actor) const override;
		virtual double _basePotencyMultiplier(const Actor* actor) const override;
		virtual double _baseAutoAttackDamage(const Actor* actor) const override;

		struct SkillSpeedUp : Aura {
			SkillSpeedUp() : Aura("skill-speed-up") {
				_statsMultiplier.skillSpeed = 1.15;
			}
			virtual std::chrono::microseconds duration() const override { return 70_s; }
		};
		SkillSpeedUp skillSpeedUp;

		struct KissOfTheWasp : Aura {
			KissOfTheWasp() : Aura("kiss-of-the-wasp") {}
			virtual std::chrono::microseconds duration() const override { return std::chrono::microseconds::max(); }
			virtual double increasedDamage() const override { return 0.2; }
		};
		KissOfTheWasp kissOfTheWasp;

		struct KissOfTheViper : Aura {
			KissOfTheViper() : Aura("kiss-of-the-viper") {}
			virtual std::chrono::microseconds duration() const override { return std::chrono::microseconds::max(); }
			virtual double increasedDamage() const override { return 0.2; }
		};
		KissOfTheViper kissOfTheViper;
};

}
