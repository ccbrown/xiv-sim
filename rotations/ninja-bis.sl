if (!AuraCount(self, "kiss-of-the-viper", self))
	use "kiss-of-the-viper";

if (GlobalCooldownRemaining(self) > 0.5) {
	if (TP(self) <= 600 and IsReady(self, "invigorate"))
		use "invigorate";

	if (IsReady(self, "blood-for-blood"))
		use "blood-for-blood";

	if (IsReady(self, "internal-release"))
		use "internal-release";
}

if (AuraTimeRemaining(target, "dancing-edge", self) and AuraTimeRemaining(target, "shadow-fang-dot", self) < 4.0 and IsReady(self, "shadow-fang-combo"))
	use "shadow-fang-combo";

if (IsReady(self, "gust-slash-combo"))
	use "gust-slash-combo";

if (AuraTimeRemaining(target, "dancing-edge", self) < 4.0 and IsReady(self, "dancing-edge-combo"))
	use "dancing-edge-combo";

if (AuraTimeRemaining(target, "dancing-edge", self) < 4.0 and IsReady(self, "spinning-edge"))
	use "spinning-edge";

if (AuraCount(self, "kassatsu-buff", self))
	use "raiton-crit";

if (IsReady(self, "trick-attack") and IsReady(self, "suiton"))
	use "suiton";

if (GlobalCooldownRemaining(self) > 0.5) {
	if (IsReady(self, "trick-attack"))
		use "trick-attack";

	if (AuraTimeRemaining(target, "trick-attack-debuff", self) and IsReady(self, "kassatsu"))
		use "kassatsu";

	if (IsReady(self, "mug"))
		use "mug";

	if (IsReady(self, "jugulate"))
		use "jugulate";
}

if (AuraTimeRemaining(target, "mutilate-dot", self) < 4.0)
	use "mutilate";

if (IsReady(self, "aeolian-edge-combo"))
	use "aeolian-edge-combo";

if (IsReady(self, "huton") and AuraTimeRemaining(self, "skill-speed-up", self) < 4.0)
	use "huton";

use "spinning-edge";
