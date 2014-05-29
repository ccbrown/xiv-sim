#include "../Action.h"
#include "../Actor.h"
#include "../JITRotation.h"
#include "../PetRotation.h"
#include "../Simulation.h"

#include "../models/Bard.h"
#include "../models/BlackMage.h"
#include "../models/Dragoon.h"
#include "../models/Garuda.h"
#include "../models/Monk.h"
#include "../models/Summoner.h"

#include <memory>
#include <cstring>
#include <map>

#include <inttypes.h>

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

	Actor::Configuration subjectConfiguration;
	subjectConfiguration.identifier = "player";
	subjectConfiguration.rotation = &subjectRotation;
	subjectConfiguration.keepsHistory = true;

	std::unique_ptr<Model> model;
	std::unique_ptr<Model> petModel;
	std::unique_ptr<Rotation> petRotation;

	if (!strcmp(argv[0], "monk")) {
		model.reset(new models::Monk());
	} else if (!strcmp(argv[0], "dragoon")) {
		model.reset(new models::Dragoon());
	} else if (!strcmp(argv[0], "black-mage")) {
		model.reset(new models::BlackMage());
	} else if (!strcmp(argv[0], "bard")) {
		model.reset(new models::Bard());
	} else if (!strcmp(argv[0], "summoner")) {
		model.reset(new models::Summoner());
		
		petModel.reset(new models::Garuda());
		petRotation.reset(new PetRotation(petModel->action("wind-blade")));
	} else {
		printf("Unknown model.\n");
		return 1;
	}

	subjectConfiguration.model = model.get();

	int simulationSeconds = 0;
	
	if (sscanf(argv[2], "WDMG=%d WDEL=%lf STR=%d DEX=%d INT=%d PIE=%d CRIT=%d SKS=%d SPS=%d DET=%d LEN=%d"
		, &subjectConfiguration.stats.weaponDamage
		, &subjectConfiguration.stats.weaponDelay
		, &subjectConfiguration.stats.strength
		, &subjectConfiguration.stats.dexterity
		, &subjectConfiguration.stats.intelligence
		, &subjectConfiguration.stats.piety
		, &subjectConfiguration.stats.criticalHitRate
		, &subjectConfiguration.stats.skillSpeed
		, &subjectConfiguration.stats.spellSpeed
		, &subjectConfiguration.stats.determination
		, &simulationSeconds
	) != 11) {
		printf("Unable to parse configuration.\n");
		return 1;
	}

	Actor::Configuration petConfiguration;
	
	if (petModel && petRotation) {
		petConfiguration.identifier = "player-pet";
		petConfiguration.model = petModel.get();
		petConfiguration.rotation = petRotation.get();
		petConfiguration.keepsHistory = subjectConfiguration.keepsHistory;
		// TODO: exclude food stats from pet stats
		petConfiguration.stats = subjectConfiguration.stats;
		subjectConfiguration.petConfiguration = &petConfiguration;
	}

	models::Monk targetModel;
	Actor::Configuration targetConfiguration;
	targetConfiguration.identifier = "target";
	targetConfiguration.model = &targetModel;
	targetConfiguration.keepsHistory = true;

	Simulation::Configuration configuration;
	configuration.length = std::chrono::seconds(simulationSeconds);
	configuration.subjectConfiguration = &subjectConfiguration;
	configuration.targetConfiguration = &targetConfiguration;

	Simulation simulation(&configuration);
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

	printf("\"length\":%" PRId64 ",\"damage\":%d,\"dps\":%f,", configuration.length.count(), mergedStats.damageDealt, mergedStats.damageDealt / (double)simulationSeconds);

	printf("\"subjects\":{");

	bool firstSubject = true;
	for (auto& subject : simulation.subjects()) {
		if (!firstSubject) { printf(","); }
		firstSubject = false;

		printf("\"%s\":{", subject->identifier().c_str());

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