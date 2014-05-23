#include "Monk.h"

#include "../Actor.h"

namespace rotations {

const Action* Monk::nextAction(const Actor* subject, const Actor* target) const {
	if (subject->isOnGlobalCooldown()) {
		return nullptr;
	}

	return &_bootshineRear;
}

}