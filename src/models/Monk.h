#pragma once

#include "../Model.h"

class Action;

namespace models {

class Monk : public Model {
	public:
		virtual Damage generatedDamage(const Action* action, const Actor* subject) const override;
		virtual Damage acceptedDamage(const Damage& incoming, const Actor* target) const override;
};

}