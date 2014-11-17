if (IsReady(self, "hawks-eye") and AuraTimeRemaining(self, "straight-shot", self) < 1.0)
	use "hawks-eye";

if (IsReady(self, "internal-release") and AuraTimeRemaining(self, "straight-shot", self) < 1.0)
	use "internal-release";

if (GlobalCooldownRemaining(self) > 0.2) {
	if (TP(self) <= 600 and IsReady(self, "invigorate"))
		use "invigorate";

	if (IsReady(self, "hawks-eye") and CooldownRemaining(self, "blood-for-blood") < 2.5 and CooldownRemaining(self, "barrage") < 7.5)
		use "hawks-eye";

	if (IsReady(self, "internal-release"))
		use "internal-release";

	if (IsReady(self, "blood-for-blood") and CooldownRemaining(self, "barrage") < 5.0)
		use "blood-for-blood";

	if (IsReady(self, "raging-strikes"))
		use "raging-strikes";

	if (IsReady(self, "barrage"))
		use "barrage";

	if (IsReady(self, "bloodletter"))
		use "bloodletter";

	if (IsReady(self, "flaming-arrow"))
		use "flaming-arrow";

	if (IsReady(self, "blunt-arrow"))
		use "blunt-arrow";

	if (IsReady(self, "repelling-shot"))
		use "repelling-shot";
}

if (AuraTimeRemaining(self, "straight-shot", self) < 2.5 or (AuraTimeRemaining(self, "straight-shot", self) < 5.0 and AuraTimeRemaining(target, "windbite-dot", self) < 5.0))
	use "straight-shot";

if (AuraTimeRemaining(target, "windbite-dot", self) < 2.0)
	use "windbite";

if (AuraTimeRemaining(target, "venomous-bite-dot", self) < 2.0)
	use "venomous-bite";

if (AuraCount(self, "straighter-shot", self))
	use "straight-shot";

use "heavy-shot";
