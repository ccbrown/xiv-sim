#include "../Actor.h"
#include "../JITRotation.h"
#include "../Simulation.h"

#include "../models/Monk.h"
#include "../models/Summoner.h"

#include <memory>
#include <cstring>

namespace applications {

int SingleJSON(int argc, const char* argv[]) {
	if (argc < 3) {
		printf("Usage: simulator single-json model rotation configuration\n");
		return 1;
	}
	
	JITRotation subjectRotation;
	if (!subjectRotation.initializeWithFile(argv[1])) {
		printf("Unable to read rotation.\n");
		return 1;
	}

	std::unique_ptr<Model> model;
	
	if (!strcmp(argv[0], "monk")) {
		model.reset(new models::Monk());
	} else if (!strcmp(argv[0], "summoner")) {
		model.reset(new models::Summoner());
	} else {
		printf("Unknown model.\n");
		return 1;
	}
	
	Actor::Configuration subjectConfiguration;
	subjectConfiguration.model = model.get();
	subjectConfiguration.rotation = &subjectRotation;

	int simulationSeconds = 0;
	
	if (sscanf(argv[2], "WDMG=%d WDEL=%lf STR=%d INT=%d PIE=%d CRIT=%d SKS=%d SPS=%d DET=%d LEN=%d"
		, &subjectConfiguration.stats.weaponDamage
		, &subjectConfiguration.stats.weaponDelay
		, &subjectConfiguration.stats.strength
		, &subjectConfiguration.stats.intelligence
		, &subjectConfiguration.stats.piety
		, &subjectConfiguration.stats.criticalHitRate
		, &subjectConfiguration.stats.skillSpeed
		, &subjectConfiguration.stats.spellSpeed
		, &subjectConfiguration.stats.determination
		, &simulationSeconds
	) != 10) {
		printf("Unable to parse configuration.\n");
		return 1;
	}

	models::Monk targetModel;
	Actor::Configuration targetConfiguration;
	targetConfiguration.model = &targetModel;

	std::random_device randomDevice;

	Simulation::Configuration configuration;
	configuration.length = std::chrono::seconds(simulationSeconds);
	configuration.rng = &randomDevice;
	configuration.subjectConfiguration = &subjectConfiguration;
	configuration.targetConfiguration = &targetConfiguration;

	Simulation simulation(&configuration);
	simulation.run();
	
	auto& stats = simulation.stats();

	printf("{");

	printf("\"length\": %d, \"damage\": %d, \"dps\": %f, ", simulationSeconds, stats.totalDamageDealt, stats.totalDamageDealt / (double)simulationSeconds);

	printf("\"effects\": [");
	bool first = true;
	for (auto& kv : simulation.statsByEffect()) {
		auto& stats = kv.second;
		if (!first) {
			printf(", ");
		}
		printf("{ \"id\": \"%s\", \"damage\": %d, \"dps\": %f, \"count\": %d, \"crits\": %d, \"avg-damage\": %f }", kv.first.c_str(), stats.totalDamageDealt, stats.totalDamageDealt / (double)simulationSeconds, stats.count, stats.criticalHits, stats.totalDamageDealt / (double)stats.count);
		first = false;
	}
	printf("]");

	printf("}");
	
	printf("\n");

	return 0;
}

}