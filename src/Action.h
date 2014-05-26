#pragma once

#include "Aura.h"
#include "common.h"

#include <chrono>
#include <vector>
#include <cassert>

class Action {
	public:
		Action(const char* identifier) : _identifier(identifier) {}
		virtual ~Action() {
			for (auto& aura : _subjectAuras) { delete aura; }
			for (auto& aura : _targetAuras) { delete aura; }
		}
	
		virtual const std::string& identifier() const { return _identifier; }

		virtual std::chrono::microseconds cooldown() const { return 0_ms; }
		virtual bool isOffGlobalCooldown() const { return false; }

		virtual int damage() const { return 0; }

		virtual double criticalHitChance(double base) const { return base; }

		virtual const std::vector<Aura*>& subjectAuras() const { return _subjectAuras; }
		virtual const std::vector<Aura*>& targetAuras() const { return _targetAuras; }

		virtual bool dispelsSubjectAura(Aura* aura) const { return false; }
		virtual bool dispelsTargetAura(Aura* aura) const { return false; }

		virtual int tpCost() const { return 0; }
		virtual int tpRestoration() const { return 0; }

		virtual int mpCost() const { return 0; }

	protected:
		const std::string _identifier;
	
		std::vector<Aura*> _subjectAuras;
		std::vector<Aura*> _targetAuras;
};