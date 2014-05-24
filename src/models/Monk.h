#pragma once

#include "../Model.h"

#include <string>
#include <unordered_map>

class Action;

namespace models {

class Monk : public Model {
	public:
		Monk();
	
		virtual Damage generateDamage(const Action* action, const Actor* actor) const override;
		virtual Damage generateAutoAttackDamage(const Actor* actor) const override;

		virtual Damage acceptDamage(const Damage& incoming, const Actor* actor) const override;

		virtual std::chrono::microseconds globalCooldown(const Actor* actor) const override;
		virtual std::chrono::microseconds autoAttackInterval(const Actor* actor) const override;

		virtual double baseTickDamage(const Actor* source, const Aura* aura) const override;
		virtual double tickCriticalHitChance(const Actor* source) const override;

		static const Action* FistsOfFire;
		static const Action* DragonKickFlankOpoOpo;
		static const Action* BootshineRear;
		static const Action* BootshineRearOpoOpo;
		static const Action* TwinSnakesFlank;
		static const Action* TrueStrikeRear;
		static const Action* SnapPunchFlank;
		static const Action* DemolishRear;
		static const Action* TouchOfDeath;
		static const Action* SteelPeak;
		static const Action* HowlingFist;
		static const Action* InternalRelease;
		static const Action* BloodForBlood;
		
	private:
		static void InitializeActions();
};

}