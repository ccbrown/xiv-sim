var isOpoOpoForm = AuraCount(self, "opo-opo-form", self) or AuraCount(self, "perfect-balance", self);
var isRaptorForm = AuraCount(self, "raptor-form", self) or AuraCount(self, "perfect-balance", self);
var isCoeurlForm = AuraCount(self, "coeurl-form", self) or AuraCount(self, "perfect-balance", self);

if (!AuraCount(self, "fists-of-fire", self))
	use "fists-of-fire";

if (TP(self) <= 600 and IsReady(self, "invigorate"))
	use "invigorate";

if (IsReady(self, "blood-for-blood"))
	use "blood-for-blood";

if (IsReady(self, "internal-release"))
	use "internal-release";

if (!AuraCount(self, "greased-lightning", self) and IsReady(self, "perfect-balance"))
	use "perfect-balance";

if (AuraCount(self, "greased-lightning", self) == 3 and AuraCount(self, "twin-snakes", self)) {
	if (IsReady(self, "steel-peak"))
		use "steel-peak";

	if (IsReady(self, "howling-fist"))
		use "howling-fist";
}

if (AuraTimeRemaining(target, "demolish-dot", self) < 4.0 and isCoeurlForm)
	use "demolish-rear";

if (TP(self) >= 300 and !AuraCount(target, "touch-of-death-dot", self))
	use "touch-of-death";

if ((AuraTimeRemaining(self, "greased-lightning", self) < 4.0 or AuraCount(self, "greased-lightning", self) < 3) and isCoeurlForm) {
	if (AuraTimeRemaining(target, "demolish-dot", self) < 4.0)
		use "demolish-rear";
	else
		use "snap-punch-flank";
}

if (isRaptorForm and AuraTimeRemaining(self, "twin-snakes", self) < 4.0)
	use "twin-snakes-flank";

if (isOpoOpoForm and AuraTimeRemaining(target, "dragon-kick", self) < 4.0 and !AuraCount(self, "perfect-balance", self))
	use "dragon-kick-flank-opo-opo";

if (isOpoOpoForm)
	use "bootshine-rear-opo-opo";

if (isRaptorForm)
	use "true-strike-rear";

if (isCoeurlForm)
	use "snap-punch-flank";

use "bootshine-rear";