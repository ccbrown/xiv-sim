if (GlobalCooldownRemaining(self) > 0.5) {

	if (TP(self) < 460 and IsReady(self, "invigorate"))
		use "invigorate";
	
	if (IsReady(self, "internal-release"))
		use "internal-release";
		
	if (IsReady(self, "blood-for-blood") and !(IsReady(self, "full-thrust-combo") and AuraTimeRemaining(target, "chaos-thrust-dot", self) < 15.0))
		use "blood-for-blood";
	
	if (IsReady(self, "power-surge") and AuraTimeRemaining(self, "blood-for-blood", self) > 3.0)
		use "power-surge";
		
	if (IsReady(self, "life-surge") and IsReady(self, "full-thrust-combo") and AuraTimeRemaining(self, "blood-for-blood", self) > 1.0)
		use "life-surge";
		
	if (IsReady(self, "jump"))
		use "jump";
	
	if (IsReady(self, "leg-sweep"))
		use "leg-sweep";
		
	if (IsReady(self, "spineshatter-dive"))
		use "spineshatter-dive";
		
	if (IsReady(self, "dragonfire-dive"))
		use "dragonfire-dive";
		
}

if (IsReady(self, "chaos-thrust-combo"))
	use "chaos-thrust-combo";
	
if (IsReady(self, "disembowel-combo"))
	use "disembowel-combo";
	
if (IsReady(self, "full-thrust-combo"))
	use "full-thrust-combo";
	
if (IsReady(self, "vorpal-thrust-combo"))
	use "vorpal-thrust-combo";

if (AuraTimeRemaining(self, "heavy-thrust", self) < 7.0)
	use "heavy-thrust-flank";

if (AuraTimeRemaining(target, "chaos-thrust-dot", self) < 6.0)
	use "impulse-drive-rear";

if (AuraTimeRemaining(target, "phlebotomize-dot", self) < 5.0)
	use "phlebotomize";

use "true-thrust";
