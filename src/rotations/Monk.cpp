#include "Monk.h"

#include "../Actor.h"

namespace rotations {

const Action* Monk::nextAction(const Actor* subject, const Actor* target) const {
	return &_bootshineRear;
}

}