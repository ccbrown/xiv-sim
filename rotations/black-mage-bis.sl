if (!CooldownRemaining(self, "raging-strikes") and ((AuraCount(self, "astral-fire", self) and MP(self) > 3000) or (AuraCount(self, "umbral-ice", self) and AuraCount(self, "thundercloud", self))))
	use "raging-strikes";

if (AuraCount(self, "thundercloud", self) and (AuraTimeRemaining(self, "umbral-ice", self) > 4.9 or AuraTimeRemaining(self, "astral-fire", self) > 4.9) and !(AuraCount(self, "astral-fire", self) and AuraTimeRemaining(self, "thundercloud", self) > 2.5 and !(!AuraCount(target, "thunder-dot", self) and AuraTimeRemaining(self, "thundercloud", self) > 4.9 and (MP(self) > 638 + 80 or AuraCount(self, "firestarter", self)))))
	use "thunder-iii";

if (AuraCount(self, "firestarter", self) and AuraCount(self, "astral-fire", self))
	use "fire-iii";

if (AuraCount(self, "firestarter", self) and AuraCount(self, "umbral-ice", self) and MP(self) > 3000)
	use "transpose";

if (MP(self) > 3000 and !AuraCount(self, "astral-fire", self))
	use "fire-iii";

if (AuraTimeRemaining(target, "thunder-dot", self) < 10.0 and (AuraCount(self, "umbral-ice", self) and MP(self) >= 319))
	use "thunder-ii";

if (AuraCount(self, "astral-fire", self) and MP(self) >= 319 * 2 + 80)
	use "fire";

if (AuraCount(self, "astral-fire", self))
	use "blizzard-iii";

use "blizzard";