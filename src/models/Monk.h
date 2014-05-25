#pragma once

#include "../Model.h"

#include <string>
#include <unordered_map>

class Action;

namespace models {

class Monk : public Model {
	public:
		Monk();
		~Monk();
	
		const Action* action(const char* identifier) const override;

		virtual Damage generateDamage(const Action* action, const Actor* actor) const override;
		virtual Damage generateAutoAttackDamage(const Actor* actor) const override;

		virtual Damage acceptDamage(const Damage& incoming, const Actor* actor) const override;

		virtual std::chrono::microseconds globalCooldown(const Actor* actor) const override;
		virtual std::chrono::microseconds autoAttackInterval(const Actor* actor) const override;

		virtual double baseTickDamage(const Actor* source, const Aura* aura) const override;
		virtual double tickCriticalHitChance(const Actor* source) const override;

	private:
		std::unordered_map<std::string, Action*> _actions;
		
		template <typename T>
		void _registerAction() {
			auto a = new T();
			_actions[a->identifier()] = a;
		}
};

}