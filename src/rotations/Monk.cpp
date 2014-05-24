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

	if (subject->globalCooldownRemaining() > 1s && subject->auraCount("greased-lightning") == 3 && subject->auraCount("twin-snakes")) {
		if (!subject->cooldownRemaining("steel-peak").count()) {
			return models::Monk::SteelPeak;
		}
		if (!subject->cooldownRemaining("howling-fist").count()) {
			return models::Monk::HowlingFist;
		}
	}

	if (!target->auraCount("touch-of-death-dot") && !subject->globalCooldownRemaining().count()) {
		return models::Monk::TouchOfDeath;
	}

	if (!target->auraCount("demolish-dot") && !IsCoeurlForm(subject) && !subject->cooldownRemaining("perfect-balance").count()) {
		return models::Monk::PerfectBalance;
	}

	if (subject->globalCooldownRemaining().count()) { return nullptr; }

	if (target->auraTimeRemaining("demolish-dot") < 4s && IsCoeurlForm(subject)) {
		return models::Monk::DemolishRear;
	}

	if ((subject->auraTimeRemaining("greased-lightning") < 4s || subject->auraCount("greased-lightning") < 3) && IsCoeurlForm(subject)) {
		return target->auraTimeRemaining("demolish-dot") < 4s ? models::Monk::DemolishRear : models::Monk::SnapPunchFlank;
	}

	if (IsRaptorForm(subject) && subject->auraTimeRemaining("twin-snakes") < 4s) {
		return models::Monk::TwinSnakesFlank;
	}

	if (IsOpoOpoForm(subject) && target->auraTimeRemaining("dragon-kick") < 4s && !subject->auraCount("perfect-balance")) {
		return models::Monk::DragonKickFlankOpoOpo;
	}

	if (IsOpoOpoForm(subject)) {
		return models::Monk::BootshineRearOpoOpo;
	}

	if (IsRaptorForm(subject)) {
		return models::Monk::TrueStrikeRear;
	}

	if (IsCoeurlForm(subject)) {
		return models::Monk::SnapPunchFlank;
	}

	return models::Monk::BootshineRear;
}

bool Monk::IsOpoOpoForm(const Actor* actor) {
	return actor->auraCount("opo-opo-form") || actor->auraCount("perfect-balance");
}

bool Monk::IsRaptorForm(const Actor* actor) {
	return actor->auraCount("raptor-form") || actor->auraCount("perfect-balance");
}

bool Monk::IsCoeurlForm(const Actor* actor) {
	return actor->auraCount("coeurl-form") || actor->auraCount("perfect-balance");
}

}