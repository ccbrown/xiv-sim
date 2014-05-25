const void* _Subject = nullptr;
const void* _Target = nullptr;

extern uint64 _AuraCount(const void* actor, const uint8* identifier) : "JITRotationActorAuraCount";
extern double _CooldownRemaining(const void* actor, const uint8* identifier) : "JITRotationActorCooldownRemaining";
extern double _AuraTimeRemaining(const void* actor, const uint8* identifier) : "JITRotationActorAuraTimeRemaining";
extern uint64 _TP(const void* actor) : "JITRotationActorTP";

uint64 AuraCount(const uint8* identifier) {
	return _AuraCount(_Subject, identifier);
}

uint64 TargetAuraCount(const uint8* identifier) {
	return _AuraCount(_Target, identifier);
}

double CooldownRemaining(const uint8* identifier) {
	return _CooldownRemaining(_Subject, identifier);
}

double AuraTimeRemaining(const uint8* identifier) {
	return _AuraTimeRemaining(_Subject, identifier);
}

double TargetAuraTimeRemaining(const uint8* identifier) {
	return _AuraTimeRemaining(_Target, identifier);
}

uint64 TP() {
	return _TP(_Subject);
}

const uint8* NextAction() {
	auto isOpoOpoForm = AuraCount("opo-opo-form") || AuraCount("perfect-balance");
	auto isRaptorForm = AuraCount("raptor-form") || AuraCount("perfect-balance");
	auto isCoeurlForm = AuraCount("coeurl-form") || AuraCount("perfect-balance");

	if (!AuraCount("fists-of-fire")) {
		return "fists-of-fire";
	}

	if (TP() <= 600 && !CooldownRemaining("invigorate")) {
		return "invigorate";
	}

	if (!CooldownRemaining("blood-for-blood")) {
		return "blood-for-blood";
	}

	if (!CooldownRemaining("internal-release")) {
		return "internal-release";
	}

	if (AuraCount("greased-lightning") == 3 && AuraCount("twin-snakes")) {
		if (!CooldownRemaining("steel-peak")) {
			return "steel-peak";
		}
		if (!CooldownRemaining("howling-fist")) {
			return "howling-fist";
		}
	}

	if (!TargetAuraCount("touch-of-death-dot")) {
		return "touch-of-death";
	}

	if (!TargetAuraCount("demolish-dot") && !isCoeurlForm && !CooldownRemaining("perfect-balance")) {
		return "perfect-balance";
	}

	if (TargetAuraTimeRemaining("demolish-dot") < 4.0 && isCoeurlForm) {
		return "demolish-rear";
	}

	if ((AuraTimeRemaining("greased-lightning") < 4.0 || AuraCount("greased-lightning") < 3) && isCoeurlForm) {
		if (TargetAuraTimeRemaining("demolish-dot") < 4.0) {
			return "demolish-rear";
		}
		return "snap-punch-flank";
	}

	if (isRaptorForm && AuraTimeRemaining("twin-snakes") < 4.0) {
		return "twin-snakes-flank";
	}

	if (isOpoOpoForm && TargetAuraTimeRemaining("dragon-kick") < 4.0 && !AuraCount("perfect-balance")) {
		return "dragon-kick-flank-opo-opo";
	}

	if (isOpoOpoForm) {
		return "bootshine-rear-opo-opo";
	}

	if (isRaptorForm) {
		return "true-strike-rear";
	}

	if (isCoeurlForm) {
		return "snap-punch-flank";
	}

	return "bootshine-rear";
}