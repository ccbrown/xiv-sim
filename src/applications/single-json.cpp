#include "ActorConfigurationParser.h"
#include "json.h"

#include "../Action.h"
#include "../Actor.h"
#include "../JITRotation.h"
#include "../Simulation.h"

#include "../models/Monk.h"

#include <memory>
#include <cstring>
#include <map>

#include <inttypes.h>

namespace applications {

int SingleJSON(int argc, const char* argv[]) {
	if (argc < 3) {
		printf("Usage: simulator single-json subject rotation length [seed]\n");
		return 1;
	}
	
	ActorConfigurationParser subjectParser;
	if (!subjectParser.parseFile(argv[0])) {
		printf("Unable to read configuration.\n");
		return 1;
	}
	
	JITRotation subjectRotation;
	if (!subjectRotation.initializeWithFile(argv[1])) {
		printf("Unable to read rotation.\n");
		return 1;
	}

	auto& subjectConfiguration = subjectParser.configuration();
	subjectConfiguration.identifier = "player";
	subjectConfiguration.rotation = &subjectRotation;
	subjectConfiguration.keepsHistory = true;

	if (subjectConfiguration.petConfiguration) {
		auto petConfiguration = *subjectConfiguration.petConfiguration;
		petConfiguration.identifier = "player-pet";
		petConfiguration.keepsHistory = subjectConfiguration.keepsHistory;
	}

	models::Monk targetModel;
	Actor::Configuration targetConfiguration;
	targetConfiguration.identifier = "target";
	targetConfiguration.model = &targetModel;
	targetConfiguration.keepsHistory = true;

	int simulationSeconds = atoi(argv[2]);

	Simulation::Configuration configuration;
	configuration.length = std::chrono::seconds(simulationSeconds);
	configuration.subjectConfiguration = &subjectConfiguration;
	configuration.targetConfiguration = &targetConfiguration;

	uint64_t seed = 0;

	if (argc < 4) {
		std::random_device rd;
		std::uniform_int_distribution<uint64_t> dist;
		seed = dist(rd);
	} else {
		seed = strtoull(argv[3], nullptr, 0);
	}

	Simulation simulation(&configuration, seed);
	simulation.run();
	
	Actor::SimulationStats mergedStats;
	std::map<std::string, Actor::EffectSimulationStats> mergedEffectStats;

	for (auto& subject : simulation.subjects()) {
		mergedStats += subject->simulationStats();
		for (auto& kv : subject->effectSimulationStats()) {
			mergedEffectStats[kv.first] += kv.second;
		}
	}

	printf("{");

	JSONPrint("seed"); printf(":"); JSONPrint(seed); printf(",");
	JSONPrint("length"); printf(":"); JSONPrint(configuration.length); printf(",");
	JSONPrint("damage"); printf(":"); JSONPrint(mergedStats.damageDealt); printf(",");

	printf("\"subjects\":{");

	bool firstSubject = true;
	for (auto& subject : simulation.subjects()) {
		if (!firstSubject) { printf(","); }
		firstSubject = false;

		JSONPrint(subject->identifier());
		printf(":");
		
		auto& simStats = subject->simulationStats();

		JSONPrintDict(
			"stats", subject->configuration()->stats,
			"damage", simStats.damageDealt,
			"effects", subject->effectSimulationStats(),
			"tp-samples", simStats.tpSamples,
			"mp-samples", simStats.mpSamples,
			"actions", simStats.actions,
			"auras", simStats.auraSamples
		);
	}

	printf("}");

	printf("}");

	printf("\n");

	return 0;
}

}