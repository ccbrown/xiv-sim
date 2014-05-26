var isOpoOpoForm = AuraCount(self, "opo-opo-form") or AuraCount(self, "perfect-balance");
var isRaptorForm = AuraCount(self, "raptor-form") or AuraCount(self, "perfect-balance");
var isCoeurlForm = AuraCount(self, "coeurl-form") or AuraCount(self, "perfect-balance");

if (!AuraCount(self, "fists-of-fire"))
	perform "fists-of-fire";

if (TP(self) <= 600 and !CooldownRemaining(self, "invigorate"))
	perform "invigorate";

if (!CooldownRemaining(self, "blood-for-blood"))
	perform "blood-for-blood";

if (!CooldownRemaining(self, "internal-release"))
	perform "internal-release";

if (AuraCount(self, "greased-lightning") == 3 and AuraCount(self, "twin-snakes")) {
	if (!CooldownRemaining(self, "steel-peak"))
		perform "steel-peak";

	if (!CooldownRemaining(self, "howling-fist"))
		perform "howling-fist";
}

if (!AuraCount(target, "touch-of-death-dot"))
	perform "touch-of-death";

if (!AuraCount(target, "demolish-dot") and !isCoeurlForm and !CooldownRemaining(self, "perfect-balance"))
	perform "perfect-balance";

if (AuraTimeRemaining(target, "demolish-dot") < 4.0 and isCoeurlForm)
	perform "demolish-rear";

if ((AuraTimeRemaining(self, "greased-lightning") < 4.0 or AuraCount(self, "greased-lightning") < 3) and isCoeurlForm) {
	if (AuraTimeRemaining(target, "demolish-dot") < 4.0)
		perform "demolish-rear";
	else
		perform "snap-punch-flank";
}

if (isRaptorForm and AuraTimeRemaining(self, "twin-snakes") < 4.0)
	perform "twin-snakes-flank";

if (isOpoOpoForm and AuraTimeRemaining(target, "dragon-kick") < 4.0 and !AuraCount(self, "perfect-balance"))
	perform "dragon-kick-flank-opo-opo";

if (isOpoOpoForm)
	perform "bootshine-rear-opo-opo";

if (isRaptorForm)
	perform "true-strike-rear";

if (isCoeurlForm)
	perform "snap-punch-flank";

perform "bootshine-rear";