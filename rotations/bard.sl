// Bard isn't done yet.

if (GlobalCooldownRemaining(self) > 1.0) {
	if (TP(self) <= 600 and IsReady(self, "invigorate"))
		use "invigorate";
	
	if (IsReady(self, "blood-for-blood"))
		use "blood-for-blood";
	
	if (IsReady(self, "internal-release"))
		use "internal-release";
}

use "straight-shot";