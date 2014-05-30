if (TP(self) < 500 and IsReady(self, "invigorate"))
	use "invigorate";

if (IsReady(self, "blood-for-blood"))
	use "blood-for-blood";

if (IsReady(self, "internal-release"))
	use "internal-release";

if (IsReady(self, "full-thrust-combo")) {
	if (IsReady(self, "life-surge"))
		use "life-surge";

	use "full-thrust-combo";
}

if (IsReady(self, "vorpal-thrust-combo"))
	use "vorpal-thrust-combo";
	
if (IsReady(self, "disembowel-combo"))
	use "disembowel-combo";
	
if (IsReady(self, "chaos-thrust-combo"))
	use "chaos-thrust-combo";

if (AuraCount(self, "power-surge", self) and IsReady(self, "jump"))
	use "jump";

if (AuraTimeRemaining(self, "heavy-thrust", self) < 4.0)
	use "heavy-thrust-flank";

if (AuraTimeRemaining(target, "phlebotomize-dot", self) < 5.0)
	use "phlebotomize";

if (AuraTimeRemaining(target, "chaos-thrust-dot", self) < 6.0)
	use "impulse-drive-rear";

if (IsReady(self, "power-surge"))
	use "power-surge";

if (IsReady(self, "dragonfire-dive"))
	use "dragonfire-dive";

use "true-thrust";
