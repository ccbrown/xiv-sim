#include "../Actor.h"
#include "../Simulation.h"

#include "../models/Monk.h"
#include "../rotations/Monk.h"

namespace applications {

int SingleHTML(int argc, const char* argv[]) {
	if (argc < 1) {
		printf("configuration required");
		return 1;
	}
	
	models::Monk subjectModel;
	rotations::Monk subjectRotation;
	Actor::Configuration subjectConfiguration;
	subjectConfiguration.model = &subjectModel;
	subjectConfiguration.rotation = &subjectRotation;

	int simulationSeconds = 0;
	
	if (sscanf(argv[0], "WDMG=%d WDEL=%lf STR=%d CRIT=%d SS=%d DET=%d LEN=%d"
		, &subjectConfiguration.stats.weaponDamage
		, &subjectConfiguration.stats.weaponDelay
		, &subjectConfiguration.stats.strength
		, &subjectConfiguration.stats.criticalHitRate
		, &subjectConfiguration.stats.skillSpeed
		, &subjectConfiguration.stats.determination
		, &simulationSeconds
	) != 7) {
		printf("unable to parse configuration\n");
		return 1;
	}

	models::Monk targetModel;
	Actor::Configuration targetConfiguration;
	targetConfiguration.model = &targetModel;

	Simulation::Configuration configuration;
	configuration.length = std::chrono::seconds(simulationSeconds);
	configuration.subjectConfiguration = &subjectConfiguration;
	configuration.targetConfiguration = &targetConfiguration;

	Simulation simulation(&configuration);
	simulation.run();
	
	auto& stats = simulation.stats();
	
	printf(
		"<style type=\"text/css\">\n"
		"td {\n"
		"	padding: 6px 40px 6px 40px;\n"
		"}\n"
		".numeric {\n"
		"	text-align: right;\n"
		"}\n"
		"</style>\n"
	);

	printf("<b>Simulation Length:</b> %d<br />\n", simulationSeconds);
	printf("<b>Overall Damage Done:</b> %d<br />\n", stats.totalDamageDealt);
	printf("<b>Overall DPS:</b> %.3f<br />\n", stats.totalDamageDealt / (double)simulationSeconds);

	printf("<br /><br />\n");
	printf("<table>\n");
		
	printf("<tr><th>Effect</th><th>Damage</th><th>DPS</th><th>Count</th><th>Crits</th><th>Average Damage</th></tr>\n");
	for (auto& kv : simulation.statsByEffect()) {
		auto& stats = kv.second;
		printf("<tr><td>%s</td><td class=\"numeric\">%d</td><td class=\"numeric\">%.3f</td><td class=\"numeric\">%d</td><td class=\"numeric\">%d</td><td class=\"numeric\">%.3f</td></tr>\n", kv.first.c_str(), stats.totalDamageDealt, stats.totalDamageDealt / (double)simulationSeconds, stats.count, stats.criticalHits, stats.totalDamageDealt / (double)stats.count);
	}

	printf("</table>\n");

	return 0;
}

}