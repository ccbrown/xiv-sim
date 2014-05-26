#pragma once

#include <chrono>
#include <string>

struct Damage;

class Aura {
	public:
		Aura(const char* identifier) : _identifier(identifier) {}
		virtual ~Aura() = default;
	
		virtual const std::string& identifier() const { return _identifier; }
	
		virtual std::chrono::microseconds duration() const = 0;
		
		virtual int maximumCount() const { return 1; }

		virtual int tickDamage() const { return 0; }
			
		virtual double increasedDamage() const { return 0.0; }
		virtual double increasedAutoAttackSpeed() const { return 0.0; }
		virtual double reducedGlobalCooldown() const { return 0.0; }
		virtual double additionalCriticalHitChance() const { return 0.0; }

		virtual void transformIncomingDamage(Damage* damage) const {}
			
		virtual bool providesImmunity(Aura* aura) const { return false; }

	private:
		const std::string _identifier;
};