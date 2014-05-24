#pragma once

#include "../Rotation.h"

namespace rotations {

class Monk : public Rotation {
	public:
		virtual const Action* nextAction(const Actor* subject, const Actor* target) const override;
		
		static bool IsOpoOpoForm(const Actor* actor);
		static bool IsRaptorForm(const Actor* actor);
		static bool IsCoeurlForm(const Actor* actor);
};

}