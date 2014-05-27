#pragma once

#include "Rotation.h"

class PetRotation : public Rotation {
	public:
		PetRotation(const Action* defaultAction) : _defaultAction(defaultAction) {}

		virtual const Action* nextAction(Actor* subject, const Actor* target) const override;

	private:
		const Action* const _defaultAction = nullptr;
};
