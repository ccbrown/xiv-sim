#include "PetRotation.h"

#include "Actor.h"
#include "Model.h"

const Action* PetRotation::nextAction(Actor* subject, const Actor* target) const {
	if (auto action = subject->command()) {
		subject->setCommand(nullptr);
		return action;
	}
	return _defaultAction;
}
