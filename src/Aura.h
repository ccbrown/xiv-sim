#pragma once

#include <chrono>
#include <string>

class Actor;
struct Damage;

class Aura {
	public:
		Aura(const char* identifier) : _identifier(identifier) {}
		virtual ~Aura() = default;
	
		virtual const std::string& identifier() const { return _identifier; }

		virtual bool isHidden() const { return false; }
	
		virtual std::chrono::microseconds duration() const = 0;
		
		virtual int maximumCount() const { return 1; }

		virtual int tickDamage() const { return 0; }
		virtual int additionalStrikesPerAutoAttack() const { return 0; }

		virtual bool shouldCancel(Actor* actor, Actor* source, int count) const { return false; }

		virtual void tick(Actor* actor, Actor* source, int count, bool isCritical) const {}
		virtual void afterEffect(Actor* actor, Actor* source, int count) const {}

		virtual double mpRegenMultiplier() const { return 1.0; }

		virtual double increasedDamage() const { return 0.0; }
		virtual double increasedAutoAttackSpeed() const { return 0.0; }
		virtual double reducedGlobalCooldown() const { return 0.0; }
		virtual double additionalCriticalHitChance() const { return 0.0; }

		virtual void transformIncomingDamage(Damage* damage) const {}
			
		virtual bool providesImmunity(const Aura* aura) const { return false; }

		struct StatsMultiplier {
			double strength = 1.0;
			double dexterity = 1.0;
			double intelligence = 1.0;
			double piety = 1.0;
			double skillSpeed = 1.0;
			double spellSpeed = 1.0;
			double criticalHitRate = 1.0;
			double determination = 1.0;
		};
		
		virtual const StatsMultiplier& statsMultiplier() const { return _statsMultiplier; }

	protected:
		StatsMultiplier _statsMultiplier;

	private:
		const std::string _identifier;
};