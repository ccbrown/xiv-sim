#pragma once

#include "Damage.h"

#include <chrono>

class Action;
class Actor;

class Model {
	public:
		virtual ~Model() = default;

		virtual Damage generatedDamage(const Action* action, const Actor* actor) const = 0;
		virtual Damage acceptedDamage(const Damage& incoming, const Actor* actor) const = 0;
		
		virtual std::chrono::microseconds globalCooldown(const Actor* actor) const = 0;
};