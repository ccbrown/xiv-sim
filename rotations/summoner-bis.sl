var garuda = Pet(self);

if (IsReady(garuda, "aerial-blast") and AuraTimeRemaining(garuda, "rouse", self) > 3.0 and AuraTimeRemaining(garuda, "spur", self) > 3.0)
	Command(garuda, "aerial-blast");
else if (!CooldownRemaining(garuda, "contagion") and AuraTimeRemaining(target, "bio-dot", self) > 10.0 and AuraTimeRemaining(target, "bio-ii-dot", self) > 10.0 and AuraTimeRemaining(target, "miasma-dot", self) > 10.0)
	Command(garuda, "contagion");

if (!CooldownRemaining(self, "raging-strikes"))
	use "raging-strikes";

var needsWeave = false;

if (!CooldownRemaining(self, "aetherflow"))
	if (GlobalCooldownRemaining(self) > 1.0)
		use "aetherflow";
	else
		needsWeave = true;

if (!CooldownRemaining(self, "rouse"))
	if (GlobalCooldownRemaining(self) > 1.0)
		use "rouse";
	else
		needsWeave = true;

if (!CooldownRemaining(self, "spur") and AuraCount(Pet(self), "rouse", self))
	if (GlobalCooldownRemaining(self) > 1.0)
		use "spur";
	else
		needsWeave = true;

if (!AuraCount(target, "bio-dot", self))
	use "bio";

if (AuraTimeRemaining(target, "miasma-dot", self) < 2.0)
	use "miasma";

if (AuraTimeRemaining(target, "bio-ii-dot", self) < 2.0)
	use "bio-ii";

if (AuraTimeRemaining(target, "shadow-flare-dot", self) < 2.0)
	use "shadow-flare";

if (AuraCount(self, "aetherflow", self) && !CooldownRemaining(self, "fester"))
	if (GlobalCooldownRemaining(self) > 1.0)
		use "fester";
	else
		needsWeave = true;

if (needsWeave)
	use "ruin-ii";

use "ruin";