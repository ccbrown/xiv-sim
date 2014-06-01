#include "ActorConfigurationParser.h"
#include "json.h"

#include "../Action.h"
#include "../Actor.h"
#include "../JITRotation.h"
#include "../Simulation.h"

#include "../common.h"

#include "../models/Monk.h"

#include <chrono>
#include <memory>
#include <cstring>
#include <map>
#include <vector>

#include <inttypes.h>

namespace applications {

int SingleJSON(int argc, const char* argv[]) {
	#define USAGE "Usage: simulator single-json --length length [--seed seed] subject rotation ...\n"

	uint64_t seed = 0;
	bool hasSeed = false;

	Simulation::Configuration configuration;
	std::vector<std::unique_ptr<ActorConfigurationParser>> parsers;
	std::vector<std::unique_ptr<JITRotation>> rotations;

	for (int i = 0; i < argc; ++i) {
		if (i == 0) {
			if (strcmp(argv[i], "--length") && strcmp(argv[i], "--seed")) {
				printf(USAGE);
				return 1;
			}
		} else if (!strcmp(argv[i - 1], "--length")) {
			configuration.length = std::chrono::duration_cast<decltype(configuration.length)>(std::chrono::duration<double>(strtod(argv[i], nullptr)));
		} else if (!strcmp(argv[i - 1], "--seed")) {
			seed = strtoull(argv[i], nullptr, 0);
			hasSeed = true;
		} else if (!strcmp(argv[i], "--length") || !strcmp(argv[i], "--seed")) {
			continue;
		} else if (parsers.size() == rotations.size()) {
			// subject
			parsers.emplace_back(new ActorConfigurationParser());
			auto& parser = parsers.back();
			if (!parser->parseFile(argv[i])) {
				printf("Unable to read configuration.\n");
				return 1;
			}
		} else {
			// rotation
			rotations.emplace_back(new JITRotation());
			auto& rotation = rotations.back();
			if (!rotation->initializeWithFile(argv[i])) {
				printf("Unable to read rotation.\n");
				return 1;
			}

			auto& parser = parsers.back();
			auto& subjectConfiguration = parser->configuration();
			subjectConfiguration.identifier = "player-" + std::to_string(parsers.size());
			subjectConfiguration.rotation = rotation.get();
			subjectConfiguration.keepsHistory = true;

			if (auto petConfiguration = parser->petConfiguration()) {
				petConfiguration->identifier = subjectConfiguration.identifier + "-pet";
				petConfiguration->keepsHistory = subjectConfiguration.keepsHistory;
			}

			configuration.subjectConfigurations.push_back(&subjectConfiguration);
		}
	}
	
	if (!configuration.length.count() || configuration.subjectConfigurations.empty() || parsers.size() != rotations.size()) {
		printf(USAGE);
		return 1;
	}

	models::Monk targetModel;
	Actor::Configuration targetConfiguration;
	targetConfiguration.identifier = "target";
	targetConfiguration.model = &targetModel;
	targetConfiguration.keepsHistory = true;

	configuration.targetConfiguration = &targetConfiguration;

	if (!hasSeed) {
		std::random_device rd;
		PortableUniformIntDistribution<uint64_t> dist;
		seed = dist(rd);
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

	JSONPrint("seed"); printf(":"); JSONPrint(std::to_string(seed)); printf(",");
	JSONPrint("time"); printf(":"); JSONPrint(configuration.length); printf(",");
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
			"model", subject->model()->identifier(),
			"pet", subject->pet() ? subject->pet()->identifier().c_str() : nullptr,
			"owner", subject->owner() ? subject->owner()->identifier().c_str() : nullptr,
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