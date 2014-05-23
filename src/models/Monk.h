#pragma once

#include "../Model.h"

class Action;

namespace models {

class Monk : public Model {
	public:
		virtual Damage generatedDamage(const Action* action, const Actor* actor) const override;
		virtual Damage acceptedDamage(const Damage& incoming, const Actor* actor) const override;
		virtual std::chrono::microseconds globalCooldown(const Actor* actor) const override;
};

}