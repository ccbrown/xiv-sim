#pragma once

#include "Rotation.h"

#include <stdint.h>

class JITRotation : public Rotation {
	public:
		bool initializeWithFile(const char* filename);

		virtual const Action* nextAction(const Actor* subject, const Actor* target) const override;

	private:
		const char*(*_jitNextAction)(const Actor* subject, const Actor* target) = nullptr;

		static uint64_t ActorAuraCount(const Actor* actor, const char* identifier);
		static double ActorCooldownRemaining(const Actor* actor, const char* identifier);
		static double ActorAuraTimeRemaining(const Actor* actor, const char* identifier);
		static uint64_t ActorTP(const Actor* actor);
};
