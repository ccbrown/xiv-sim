#include "ActorConfigurationParser.h"

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

	printf("\"seed\":\"%" PRIu64 "\",\"length\":%" PRId64 ",\"damage\":%d,\"dps\":%f,", seed, configuration.length.count(), mergedStats.damageDealt, mergedStats.damageDealt / (double)simulationSeconds);

	printf("\"subjects\":{");

	bool firstSubject = true;
	for (auto& subject : simulation.subjects()) {
		if (!firstSubject) { printf(","); }
		firstSubject = false;

		printf("\"%s\":{", subject->identifier().c_str());

		auto configuration = subject->configuration();

		printf("\"stats\":{");
		printf("\"wpdmg\":%d,", configuration->stats.weaponPhysicalDamage);
		printf("\"wmdmg\":%d,", configuration->stats.weaponMagicDamage);
		printf("\"wdel\":%f,", configuration->stats.weaponDelay);
		printf("\"str\":%d,", configuration->stats.strength);
		printf("\"dex\":%d,", configuration->stats.dexterity);
		printf("\"int\":%d,", configuration->stats.intelligence);
		printf("\"pie\":%d,", configuration->stats.piety);
		printf("\"crt\":%d,", configuration->stats.criticalHitRate);
		printf("\"det\":%d,", configuration->stats.determination);
		printf("\"sks\":%d,", configuration->stats.skillSpeed);
		printf("\"sps\":%d", configuration->stats.spellSpeed);
		printf("},");

		auto& stats = subject->simulationStats();
		
		printf("\"damage\":%d,\"dps\":%f,", stats.damageDealt, stats.damageDealt / (double)simulationSeconds);
		
		auto& effectStats = subject->effectSimulationStats();

		{
			printf("\"effects\":[");
			bool first = true;
			for (auto& kv : effectStats) {
				if (!first) { printf(","); }
				printf("{\"id\":\"%s\",\"damage\":%d,\"dps\":%f,\"count\":%d,\"crits\":%d,\"avg-damage\":%f}", kv.first.c_str(), kv.second.damageDealt, kv.second.damageDealt / (double)simulationSeconds, kv.second.count, kv.second.criticalHits, kv.second.damageDealt / (double)kv.second.count);
				first = false;
			}
			printf("],");
		}
	
		{
			printf("\"tp-samples\":[");
			bool first = true;
			for (auto& sample : stats.tpSamples) {
				if (!first) { printf(","); }
				printf("[%" PRId64 ",%d]", sample.first.count(), sample.second);
				first = false;
			}
			printf("],");
		}
	
		{
			printf("\"mp-samples\":[");
			bool first = true;
			for (auto& sample : stats.mpSamples) {
				if (!first) { printf(","); }
				printf("[%" PRId64 ",%d]", sample.first.count(), sample.second);
				first = false;
			}
			printf("],");
		}
	
		{
			printf("\"auras\":{");
			bool first = true;
			for (auto& aura : stats.auraSamples) {
				if (!first) { printf(","); }
				printf("\"%s\":", aura.first.c_str());
				printf("[");
				bool firstSample = true;
				for (auto& sample : aura.second) {
					if (!firstSample) {
						printf(",");
					}
					printf("[%" PRId64 ",%d]", sample.first.count(), sample.second);
					firstSample = false;
				}
				printf("]");
				first = false;
			}
			printf("},");
		}

		{
			printf("\"actions\":[");
			bool first = true;
			for (auto& action : stats.actions) {
				if (!first) { printf(","); }
				printf("\"%s\"", action->identifier().c_str());
				first = false;
			}
			printf("]");
		}

		printf("}");
	}

	printf("}");

	printf("}");

	printf("\n");

	return 0;
}

}