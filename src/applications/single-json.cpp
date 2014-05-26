#include "../Actor.h"
#include "../JITRotation.h"
#include "../Simulation.h"

#include "../models/Monk.h"

namespace applications {

int SingleJSON(int argc, const char* argv[]) {
	if (argc < 2) {
		printf("Rotation and configuration required.\n");
		return 1;
	}
	
	JITRotation subjectRotation;
	if (!subjectRotation.initializeWithFile(argv[0])) {
		printf("Unable to read rotation.\n");
		return 1;
	}
	
	models::Monk subjectModel;
	Actor::Configuration subjectConfiguration;
	subjectConfiguration.model = &subjectModel;
	subjectConfiguration.rotation = &subjectRotation;

	int simulationSeconds = 0;
	
	if (sscanf(argv[1], "WDMG=%d WDEL=%lf STR=%d CRIT=%d SS=%d DET=%d LEN=%d"
		, &subjectConfiguration.stats.weaponDamage
		, &subjectConfiguration.stats.weaponDelay
		, &subjectConfiguration.stats.strength
		, &subjectConfiguration.stats.criticalHitRate
		, &subjectConfiguration.stats.skillSpeed
		, &subjectConfiguration.stats.determination
		, &simulationSeconds
	) != 7) {
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