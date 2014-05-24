#include "Monk.h"

#include "../Actor.h"
#include "../models/Monk.h"

namespace rotations {

const Action* Monk::nextAction(const Actor* subject, const Actor* target) const {
	if (!subject->auraCount("fists-of-fire")) {
		return models::Monk::FistsOfFire;
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

	if (!target->auraCount("touch-of-death-dot")) {
		return models::Monk::TouchOfDeath;
	} else if (subject->auraCount("raptor-form") && subject->auraTimeRemaining("twin-snakes") < 5s) {
		return models::Monk::TwinSnakesFlank;
	} else if (subject->auraCount("raptor-form")) {
		return models::Monk::TrueStrikeRear;
	} else if (subject->auraCount("coeurl-form") && target->auraTimeRemaining("demolish-dot") < 5s) {
		return models::Monk::DemolishRear;
	} else if (subject->auraCount("coeurl-form")) {
		return models::Monk::SnapPunchFlank;
	} else if (subject->auraCount("opo-opo-form") && target->auraTimeRemaining("dragon-kick") < 5s) {
		return models::Monk::DragonKickFlankOpoOpo;
	} else if (subject->auraCount("opo-opo-form")) {
		return models::Monk::BootshineRearOpoOpo;
	}

	return models::Monk::BootshineRear;
}

}