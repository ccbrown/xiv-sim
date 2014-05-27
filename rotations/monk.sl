var isOpoOpoForm = AuraCount(self, "opo-opo-form", self) or AuraCount(self, "perfect-balance", self);
var isRaptorForm = AuraCount(self, "raptor-form", self) or AuraCount(self, "perfect-balance", self);
var isCoeurlForm = AuraCount(self, "coeurl-form", self) or AuraCount(self, "perfect-balance", self);

if (!AuraCount(self, "fists-of-fire", self))
	perform "fists-of-fire";

if (TP(self) <= 600 and !CooldownRemaining(self, "invigorate"))
	perform "invigorate";

if (!CooldownRemaining(self, "blood-for-blood"))
	perform "blood-for-blood";

if (!CooldownRemaining(self, "internal-release"))
	perform "internal-release";

if (!AuraCount(self, "greased-lightning", self) and !CooldownRemaining(self, "perfect-balance"))
	perform "perfect-balance";

if (AuraCount(self, "greased-lightning", self) == 3 and AuraCount(self, "twin-snakes", self)) {
	if (!CooldownRemaining(self, "steel-peak"))
		perform "steel-peak";

	if (!CooldownRemaining(self, "howling-fist"))
		perform "howling-fist";
}

if (AuraTimeRemaining(target, "demolish-dot", self) < 4.0 and isCoeurlForm)
	perform "demolish-rear";

if (TP(self) >= 300 and !AuraCount(target, "touch-of-death-dot", self))
	perform "touch-of-death";

if ((AuraTimeRemaining(self, "greased-lightning", self) < 4.0 or AuraCount(self, "greased-lightning", self) < 3) and isCoeurlForm) {
	if (AuraTimeRemaining(target, "demolish-dot", self) < 4.0)
		perform "demolish-rear";
	else
		perform "snap-punch-flank";
}

if (isRaptorForm and AuraTimeRemaining(self, "twin-snakes", self) < 4.0)
	perform "twin-snakes-flank";

if (isOpoOpoForm and AuraTimeRemaining(target, "dragon-kick", self) < 4.0 and !AuraCount(self, "perfect-balance", self))
	perform "dragon-kick-flank-opo-opo";

if (isOpoOpoForm)
	perform "bootshine-rear-opo-opo";

if (isRaptorForm)
	perform "true-strike-rear";

if (isCoeurlForm)
	perform "snap-punch-flank";

perform "bootshine-rear";