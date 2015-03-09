if (!AuraCount(self, "kiss-damage-buff", self))
	use "kiss-of-the-wasp";

if (GlobalCooldownRemaining(self) > 0.5) {
	if (IsReady(self, "huton") and AuraTimeRemaining(self, "skill-speed-up", self) < 5.0)
		use "huton";

	if (TP(self) <= 600 and IsReady(self, "invigorate"))
		use "invigorate";

	if (IsReady(self, "blood-for-blood"))
		use "blood-for-blood";

	if (IsReady(self, "internal-release"))
		use "internal-release";

	if (CooldownRemaining(self, "trick-attack") < 3.0 and IsReady(self, "suiton"))
		use "suiton";

	if (IsReady(self, "raiton") and AuraTimeRemaining(self, "skill-speed-up", self) > 17.0 and CooldownRemaining(self, "trick-attack") > 17.0)
		use "raiton";

	if (IsReady(self, "trick-attack"))
		use "trick-attack";

	if (AuraTimeRemaining(target, "trick-attack-debuff", self) and IsReady(self, "kassatsu"))
		use "kassatsu";

	if (IsReady(self, "raiton-crit"))
		use "raiton-crit";
}

if (AuraTimeRemaining(target, "shadow-fang-dot", self) < 4.0 and IsReady(self, "shadow-fang-combo"))
	use "shadow-fang-combo";

if (IsReady(self, "gust-slash-combo"))
	use "gust-slash-combo";

if (AuraTimeRemaining(target, "dancing-edge", self) < 4.0 and IsReady(self, "dancing-edge-combo"))
	use "dancing-edge-combo";

if (IsReady(self, "aeolian-edge-combo"))
	use "aeolian-edge-combo";

if (AuraTimeRemaining(target, "mutilate-dot", self) < 2.0)
	use "mutilate";

if (GlobalCooldownRemaining(self) > 0.5) {
	if (IsReady(self, "mug"))
		use "mug";

	if (IsReady(self, "jugulate"))
		use "jugulate";
}

use "spinning-edge";
