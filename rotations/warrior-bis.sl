if (!AuraCount(self, "defiance", self))
	use "defiance";

if (AuraCount(self, "wrath", self) == 0 && IsReady(self, "infuriate"))
	use "infuriate";

if (IsReady(self, "tomahawk"))
	use "tomahawk";

if (GlobalCooldownRemaining(self) > 0.5) {
	if (IsReady(self, "brutal-swing"))
		use "brutal-swing";

	if (IsReady(self, "unchained"))
		use "unchained";

	if (IsReady(self, "berserk"))
		use "berserk";

	if (IsReady(self, "internal-release"))
		use "internal-release";
}

if (AuraTimeRemaining(target, "storms-eye-debuff", self) < 4.0 && IsReady(self, "storms-eye-combo"))
	use "storms-eye-combo";

if (IsReady(self, "butchers-block-combo"))
	use "butchers-block-combo";

if (AuraTimeRemaining(target, "maim-debuff", self) < 4.0 && IsReady(self, "maim-combo"))
	use "maim-combo";

if (IsReady(self, "skull-sunder-combo"))
	use "skull-sunder-combo";

if (!IsReady(self, "unchained") && AuraTimeRemaining(target, "fracture-dot", self) < 4.0)
	use "fracture";

if (!IsReady(self, "unchained") && !IsReady(self, "infuriate") && IsReady(self, "inner-beast"))
	use "inner-beast";

use "heavy-swing";
