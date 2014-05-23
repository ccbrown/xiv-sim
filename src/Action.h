#pragma once

#include "Aura.h"

#include <chrono>
#include <vector>
#include <cassert>

using namespace std::literals::chrono_literals;

class Action {
	public:
		Action(const char* identifier) : _identifier(identifier) {}
		virtual ~Action() {
			for (auto& aura : _subjectAuras) { delete aura; }
			for (auto& aura : _targetAuras) { delete aura; }
		}
	
		virtual const std::string& identifier() const { return _identifier; }

		virtual std::chrono::milliseconds cooldown() const { return 0ms; }
		virtual bool isAllowedDuringGlobalCooldown() const { return false; }
		virtual bool triggersGlobalCooldown() const { return true; }

		virtual int damage() const { return 0; }

		virtual double criticalHitChance(double base) const { return base; }

		virtual const std::vector<Aura*>& subjectAuras() const { return _subjectAuras; }
		virtual const std::vector<Aura*>& targetAuras() const { return _targetAuras; }
			
	protected:
		const std::string _identifier;
	
		std::vector<Aura*> _subjectAuras;
		std::vector<Aura*> _targetAuras;
};