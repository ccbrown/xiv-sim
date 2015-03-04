if (AuraCount(self, "swiftcast", self))
	use "shadow-flare";

if (IsReady(self, "raging-strikes")) {
	use "raging-strikes";
}

var garuda = Pet(self);
var needsWeave = false;
var globalCooldownActive = GlobalCooldownRemaining(self) > 0.8;

if (IsReady(garuda, "aerial-blast") and AuraTimeRemaining(garuda, "rouse", self) > 3.0 and AuraTimeRemaining(garuda, "spur", self) > 3.0)
	Command(garuda, "aerial-blast");
else if (!CooldownRemaining(garuda, "contagion") and AuraTimeRemaining(target, "bio-dot", self) > 10.0 and AuraTimeRemaining(target, "bio-ii-dot", self) > 10.0 and AuraTimeRemaining(target, "miasma-dot", self) > 10.0)
	Command(garuda, "contagion");

if (!CooldownRemaining(self, "aetherflow") and !AuraCount(self, "aetherflow", self)) {
	if (globalCooldownActive) {
		use "aetherflow";
	}
	needsWeave = true;
}

if (IsReady(self, "energy-drain") and MP(self) < 1500) {
	if (globalCooldownActive) {
		use "energy-drain";
	}
	needsWeave = true;
}

if (IsReady(self, "rouse")) {
	if (globalCooldownActive) {
		use "rouse";
	}
	needsWeave = true;
}

if (IsReady(self, "spur") and AuraCount(Pet(self), "rouse", self)) {
	if (globalCooldownActive) {
		use "spur";
	}
	needsWeave = true;
}

if (!AuraCount(target, "bio-dot", self))
	needsWeave = false;

if (!needsWeave) {
	if (!AuraCount(target, "bio-dot", self))
		use "bio";

	if (AuraTimeRemaining(target, "miasma-dot", self) < 2.0)
		use "miasma";

	if (AuraTimeRemaining(target, "bio-ii-dot", self) < 2.0)
		use "bio-ii";

	if (AuraTimeRemaining(target, "shadow-flare-dot", self) < 2.0) {
		if (CooldownRemaining(self, "swiftcast") > 10.0) {
			use "shadow-flare";
		} else if (!CooldownRemaining(self, "swiftcast")) {
			if (globalCooldownActive) {
				use "swiftcast";
			}
			needsWeave = true;
		}
	}
}

if (IsReady(self, "fester")) {
	if (globalCooldownActive) {
		use "fester";
	}
	needsWeave = true;
}

if (needsWeave or MP(self) > 1500)
	use "ruin-ii";

use "ruin";
