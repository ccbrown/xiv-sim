var garuda = Pet(self);

if (!CooldownRemaining(garuda, "contagion") and AuraTimeRemaining(target, "bio-dot", self) > 10.0 and AuraTimeRemaining(target, "bio-ii-dot", self) > 10.0 and AuraTimeRemaining(target, "miasma-dot", self) > 10.0)
	Command(garuda, "contagion");

if (!CooldownRemaining(self, "aetherflow"))
	perform "aetherflow";

if (!CooldownRemaining(self, "rouse"))
	perform "rouse";

if (!CooldownRemaining(self, "spur"))
	perform "spur";

if (!AuraCount(target, "bio-dot", self))
	perform "bio";

if (AuraTimeRemaining(target, "miasma-dot", self) < 2.0)
	perform "miasma";

if (AuraTimeRemaining(target, "bio-ii-dot", self) < 2.0)
	perform "bio-ii";

if (AuraCount(self, "aetherflow", self) && !CooldownRemaining(self, "fester"))
	perform "fester";

if (MP(self) > 500)
	perform "ruin-ii";

perform "ruin";