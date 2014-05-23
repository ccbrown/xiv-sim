#pragma once

#include "Damage.h"

class Action;
class Actor;

class Model {
	public:
		virtual ~Model() = default;

		virtual Damage generatedDamage(const Action* action, const Actor* subject) const = 0;
		virtual Damage acceptedDamage(const Damage& incoming, const Actor* target) const = 0;
};