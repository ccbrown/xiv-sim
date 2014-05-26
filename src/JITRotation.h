#pragma once

#include "Rotation.h"

class JITRotation : public Rotation {
	public:
		bool initializeWithFile(const char* filename);

		virtual const Action* nextAction(const Actor* subject, const Actor* target) const override;

	private:
		const char*(*_jitNextAction)(const Actor* subject, const Actor* target) = nullptr;
};
