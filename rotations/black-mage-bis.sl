if (!CooldownRemaining(self, "raging-strikes"))
	use "raging-strikes";

if (AuraCount(self, "firestarter", self) and AuraCount(self, "astral-fire", self))
	use "fire-iii";

if (AuraCount(self, "firestarter", self) and AuraCount(self, "umbral-ice", self) and MP(self) > 3000)
	use "transpose";

if (MP(self) > 3000 and !AuraCount(self, "astral-fire", self))
	use "fire-iii";

if (AuraCount(self, "thundercloud", self) and (AuraTimeRemaining(self, "umbral-ice", self) > 3.0 or AuraTimeRemaining(self, "astral-fire", self) > 3.0))
	use "thunder-iii";

if (AuraTimeRemaining(target, "thunder-dot", self) < 10.0 and (AuraCount(self, "umbral-ice", self) and MP(self) >= 319))
	use "thunder-ii";

if (AuraCount(self, "astral-fire", self) and MP(self) >= 319 * 2 + 80)
	use "fire";

if (AuraCount(self, "astral-fire", self))
	use "blizzard-iii";

return nullptr;