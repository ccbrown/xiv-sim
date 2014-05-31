if (GlobalCooldownRemaining(self) > 1.0) {
	if (IsReady(self, "bloodletter"))
		use "bloodletter";

	if (IsReady(self, "flaming-arrow"))
		use "flaming-arrow";

	if (TP(self) <= 600 and IsReady(self, "invigorate"))
		use "invigorate";
	
	if (IsReady(self, "blood-for-blood"))
		use "blood-for-blood";

	if (IsReady(self, "internal-release"))
		use "internal-release";

	if (IsReady(self, "barrage"))
		use "barrage";

	if (IsReady(self, "hawks-eye"))
		use "hawks-eye";

	if (IsReady(self, "repelling-shot"))
		use "repelling-shot";

	if (IsReady(self, "blunt-arrow"))
		use "blunt-arrow";
}

if (AuraTimeRemaining(target, "windbite-dot", self) < 2.0)
	use "windbite";

if (AuraTimeRemaining(target, "venomous-bite-dot", self) < 2.0)
	use "venomous-bite";

if (AuraTimeRemaining(self, "straight-shot", self) < 2.5)
	use "straight-shot";

use "heavy-shot";
