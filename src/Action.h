#pragma once

#include "common.h"

#include <chrono>
#include <vector>
#include <cassert>

class Actor;
class Aura;

class Action {
	public:
		Action(const char* identifier) : _identifier(identifier) {}
		virtual ~Action();
		
		bool resolve(Actor* subject, Actor* target) const;

		virtual const std::string& identifier() const { return _identifier; }

		virtual std::chrono::microseconds castTime() const { return 0_us; }
		virtual std::chrono::microseconds cooldown() const { return 0_us; }
		virtual bool isOffGlobalCooldown() const { return false; }

		virtual int damage(const Actor* source, const Actor* target) const { return damage(); }

		virtual double criticalHitChance(double base) const { return base; }

		virtual const std::vector<Aura*>& subjectAuras() const { return _subjectAuras; }
		virtual const std::vector<Aura*>& targetAuras() const { return _targetAuras; }

		virtual int dispelsSubjectAura(const Aura* aura) const { return 0; }
		virtual int dispelsTargetAura(const Aura* aura) const { return 0; }

		virtual int tpCost() const { return 0; }
		virtual int tpRestoration() const { return 0; }

		virtual int mpCost() const { return 0; }
		virtual int mpRestoration(const Actor* subject) const { return 0; }

	protected:
		const std::string _identifier;
	
		std::vector<Aura*> _subjectAuras;
		std::vector<Aura*> _targetAuras;

		virtual int damage() const { return 0; }
		virtual void resolution(Actor* source, Actor* target) const {}
};