if (TP(self) < 500 and IsReady(self, "invigorate"))
	perform "invigorate";

if (IsReady(self, "blood-for-blood"))
	perform "blood-for-blood";

if (IsReady(self, "internal-release"))
	perform "internal-release";

if (IsReady(self, "full-thrust-combo")) {
	if (IsReady(self, "life-surge"))
		perform "life-surge";
	else if (AuraCount(self, "life-surge", self))
		perform "full-thrust-combo";
}

if (IsReady(self, "vorpal-thrust-combo"))
	perform "vorpal-thrust-combo";
	
if (IsReady(self, "disembowel-combo"))
	perform "disembowel-combo";
	
if (IsReady(self, "chaos-thrust-combo"))
	perform "chaos-thrust-combo";

if (AuraCount(self, "power-surge", self) and IsReady(self, "jump"))
	perform "jump";

if (AuraTimeRemaining(self, "heavy-thrust", self) < 4.0)
	perform "heavy-thrust-flank";

if (AuraTimeRemaining(target, "fracture-dot", self) < 4.0)
	perform "fracture";

if (AuraTimeRemaining(target, "phlebotomize-dot", self) < 4.0)
	perform "phlebotomize";

if (AuraTimeRemaining(target, "chaos-thrust-dot", self) < 5.0)
	perform "impulse-drive-rear";

if (IsReady(self, "power-surge"))
	perform "power-surge";

if (IsReady(self, "dragonfire-dive"))
	perform "dragonfire-dive";

perform "true-thrust";
