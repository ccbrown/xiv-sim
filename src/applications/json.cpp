#include "json.h"

#include "../Action.h"

namespace applications {

void JSONPrint(const char* string) {
	// TODO: make sure the string is properly escaped
	printf("\"%s\"", string);
}

void JSONPrint(const std::string& string) {
	JSONPrint(string.c_str());
}

void JSONPrint(int32_t n) {
	printf("%d", n);
}

void JSONPrint(int64_t n) {
	printf("%" PRId64, n);
}

void JSONPrint(uint64_t n) {
	printf("%" PRIu64, n);
}

void JSONPrint(double f) {
	printf("%f", f);
}

void JSONPrint(const Actor::Stats& actorStats) {
	JSONPrintDict(
		"wpdmg", actorStats.weaponPhysicalDamage,
		"wmdmg", actorStats.weaponMagicDamage,
		"wdel", actorStats.weaponDelay,
		"str", actorStats.strength,
		"dex", actorStats.dexterity,
		"int", actorStats.intelligence,
		"pie", actorStats.piety,
		"crt", actorStats.criticalHitRate,
		"det", actorStats.determination,
		"sks", actorStats.skillSpeed,
		"sps", actorStats.spellSpeed
	);
}

void JSONPrint(const Actor::EffectSimulationStats& effectStats) {
	JSONPrintDict("damage", effectStats.damageDealt, "count", effectStats.count, "crits", effectStats.criticalHits);
}

void JSONPrint(const Action* action) {
	JSONPrint(action->identifier());
}

} // namespace applications