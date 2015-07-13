if (GlobalCooldownRemaining(self) > 1.0) {
	if (IsReady(self, "bloodletter"))
		use "bloodletter";

	if (IsReady(self, "flaming-arrow"))
		use "flaming-arrow";

	if (TP(self) <= 600 and IsReady(self, "invigorate"))
		use "invigorate";
	
	if (IsReady(self, "raging-strikes"))
		use "raging-strikes";

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

if (AuraTimeRemaining(self, "straight-shot", self) < 2.0)
	use "straight-shot";

if (AuraCount(target, "windbite-dot", self) and AuraTimeRemaining(target, "windbite-dot", self) < 4.5 and AuraTimeRemaining(self, "straight-shot", self) < 4.5)
	use "straight-shot";

if (AuraCount(target, "venomous-bite-dot", self) and AuraTimeRemaining(target, "venomous-bite-dot", self) < 4.5 and AuraTimeRemaining(self, "straight-shot", self) < 4.5)
	use "straight-shot";

if (AuraTimeRemaining(target, "windbite-dot", self) < 2.0)
	use "windbite";

if (AuraTimeRemaining(target, "venomous-bite-dot", self) < 2.0)
	use "venomous-bite";

if (AuraCount(target, "straighter-shot", self))
	use "straight-shot";

use "heavy-shot";
