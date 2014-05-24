#include "Monk.h"

#include "../Actor.h"
#include "../models/Monk.h"

namespace rotations {

const Action* Monk::nextAction(const Actor* subject, const Actor* target) const {
	if (!subject->auraCount("fists-of-fire")) {
		return models::Monk::FistsOfFire;
	}

	if (subject->tp() <= 600 && !subject->cooldownRemaining("invigorate").count()) {
		return models::Monk::Invigorate;
	}

	if (!subject->cooldownRemaining("blood-for-blood").count()) {
		return models::Monk::BloodForBlood;
	}

	if (!subject->cooldownRemaining("internal-release").count()) {
		return models::Monk::InternalRelease;
	}

	if (subject->globalCooldownRemaining() > 1s) {
		if (!subject->cooldownRemaining("steel-peak").count()) {
			return models::Monk::SteelPeak;
		}
		if (!subject->cooldownRemaining("howling-fist").count()) {
			return models::Monk::HowlingFist;
		}
	}

	if (subject->globalCooldownRemaining().count()) { return nullptr; }

	if (!target->auraCount("demolish-dot") && !subject->auraCount("coeurl-form") && !subject->cooldownRemaining("perfect-balance").count()) {
		return models::Monk::PerfectBalance;
	}

	if (target->auraTimeRemaining("demolish-dot") > 10s && target->auraTimeRemaining("touch-of-death-dot") > 10s) {
		return models::Monk::PerfectBalance;
	}

	if (target->auraTimeRemaining("demolish-dot") < 4s && (subject->auraCount("coeurl-form") || subject->auraCount("perfect-balance"))) {
		return models::Monk::DemolishRear;
	}

	if (!target->auraCount("touch-of-death-dot")) {
		return models::Monk::TouchOfDeath;
	}

	if (subject->auraTimeRemaining("twin-snakes") < 4s && (subject->auraCount("raptor-form") || subject->auraCount("perfect-balance"))) {
		return models::Monk::TwinSnakesFlank;
	}
	
	if (subject->auraCount("coeurl-form") || (subject->auraCount("greased-lightning") < 3 && subject->auraCount("perfect-balance"))) {
		return models::Monk::SnapPunchFlank;
	}
	
	if (subject->auraCount("perfect-balance")) {
		return models::Monk::TrueStrikeRear;
	}

	if (subject->auraCount("opo-opo-form") && target->auraTimeRemaining("dragon-kick") < 5s) {
		return models::Monk::DragonKickFlankOpoOpo;
	}
	
	if (subject->auraCount("opo-opo-form")) {
		return models::Monk::BootshineRearOpoOpo;
	}

	if (subject->auraCount("raptor-form")) {
		return models::Monk::TrueStrikeRear;
	}

	return models::Monk::BootshineRear;
}

}