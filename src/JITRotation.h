#pragma once

#include "Rotation.h"

class JITRotation : public Rotation {
	public:
		bool initializeWithFile(const char* filename);

		virtual const Action* nextAction(const Actor* subject, const Actor* target) const override;

	private:
		const Actor** _jitSubject = nullptr;
		const Actor** _jitTarget = nullptr;
		const char*(*_jitNextAction)() = nullptr;
};
