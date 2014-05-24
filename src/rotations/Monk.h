#pragma once

#include "../Rotation.h"

namespace rotations {

class Monk : public Rotation {
	public:
		virtual const Action* nextAction(const Actor* subject, const Actor* target) const override;
};

}