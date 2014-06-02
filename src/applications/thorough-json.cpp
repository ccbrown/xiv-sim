#include "ActorConfigurationParser.h"
#include "json.h"

#include "../Actor.h"
#include "../JITRotation.h"
#include "../Simulation.h"

#include "../models/Dragoon.h"

#include <future>
#include <vector>
#include <map>
#include <inttypes.h>

namespace applications {

struct TrialResults {
	uint64_t seed;
	Simulation::Configuration configuration;
	Actor::SimulationStats overallStats;
	std::unordered_map<std::string, Actor::EffectSimulationStats> effectStats;
};

static TrialResults Trial(const Simulation::Configuration&& configuration) {
	TrialResults ret;
	ret.configuration = configuration;

	{
		std::random_device rd;
		PortableUniformIntDistribution<uint64_t> dist;
		ret.seed = dist(rd);
	}

	Simulation simulation(&ret.configuration, ret.seed);
	simulation.run();

	for (auto& subject : simulation.subjects()) {
		ret.overallStats += subject->simulationStats();
		for (auto& kv : subject->effectSimulationStats()) {
			ret.effectStats[kv.first] += kv.second;
		}
	}

	return ret;
}

struct SimulationStats {
	int iterations = 0;
	uint64_t worstSeed = 0;
	double worstDPS = 0.0;
	std::chrono::microseconds worstTime = 0_us;
	uint64_t bestSeed = 0;
	double bestDPS = 0.0;
	std::chrono::microseconds bestTime = 0_us;
	std::chrono::microseconds time = 0_us;
	Actor::SimulationStats general;
	std::map<std::string, Actor::EffectSimulationStats> effects;
};

void PerformSimulations(int iterations, const std::vector<Actor::Configuration*>& subjectConfigurations, Actor::Configuration* targetConfiguration, std::chrono::microseconds minTime, std::chrono::microseconds maxTime, SimulationStats* stats) {
	stats->iterations = iterations;
	
	int remaining = iterations;

	while (remaining) {
		std::vector<std::future<TrialResults>> futures;
	
		for (int i = 0; remaining && i < 5000; ++i, --remaining) {
			Simulation::Configuration configuration;
			configuration.length = std::chrono::duration_cast<std::chrono::microseconds>(minTime + (i % 50) / 50.0 * (maxTime - minTime));
			configuration.subjectConfigurations = subjectConfigurations;
			configuration.targetConfiguration = targetConfiguration;
			
			futures.emplace_back(std::async(std::launch::async, Trial, std::move(configuration)));
		}

		for (auto& future : futures) {
			auto results = future.get();
			
			stats->general += results.overallStats;
			for (auto& kv : results.effectStats) {
				stats->effects[kv.first] += kv.second;
			}
			stats->time += results.configuration.length;
	
			auto dps = results.overallStats.damageDealt / std::chrono::duration<double>(results.configuration.length).count();
			if (!stats->worstDPS || dps < stats->worstDPS) {
				stats->worstSeed = results.seed;
				stats->worstDPS = dps;
				stats->worstTime = results.configuration.length;
			}
			if (!stats->bestDPS || dps > stats->bestDPS) {
				stats->bestSeed = results.seed;
				stats->bestDPS = dps;
				stats->bestTime = results.configuration.length;
			}
		}
	}
}

int ThoroughJSON(int argc, const char* argv[]) {	
	if (argc < 4) {
		printf("Usage: simulator thorough-json subject rotation min-length max-length\n");
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
	
	auto minLength = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::duration<double>(strtod(argv[2], nullptr)));
	auto maxLength = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::duration<double>(strtod(argv[3], nullptr)));

	if (minLength <= 0_us || maxLength <= minLength) {
		printf("Invalid lengths.");
		return 1;
	}

	std::vector<Actor::Configuration*> subjectConfigurations;

	auto& subjectConfiguration = subjectParser.configuration();
	subjectConfiguration.identifier = "player";
	subjectConfiguration.rotation = &subjectRotation;
	subjectConfigurations.push_back(&subjectConfiguration);

	if (subjectConfiguration.petConfiguration) {
		auto petConfiguration = *subjectConfiguration.petConfiguration;
		petConfiguration.identifier = "player-pet";
		petConfiguration.keepsHistory = subjectConfiguration.keepsHistory;
	}

	models::Dragoon targetModel;
	Actor::Configuration targetConfiguration;
	targetConfiguration.identifier = "target";
	targetConfiguration.model = &targetModel;

	printf("{");

	SimulationStats unmodifiedStats;
	PerformSimulations(100000, subjectConfigurations, &targetConfiguration, minLength, maxLength, &unmodifiedStats);

	JSONPrint("iterations"); printf(":"); JSONPrint(unmodifiedStats.iterations); printf(",");
	JSONPrint("min-time"); printf(":"); JSONPrint(minLength); printf(",");
	JSONPrint("max-time"); printf(":"); JSONPrint(maxLength); printf(",");
	JSONPrint("time"); printf(":"); JSONPrint(unmodifiedStats.time); printf(",");
	JSONPrint("damage"); printf(":"); JSONPrint(unmodifiedStats.general.damageDealt); printf(",");
	JSONPrint("best-dps"); printf(":"); JSONPrint(unmodifiedStats.bestDPS); printf(",");
	JSONPrint("best-seed"); printf(":"); JSONPrint(std::to_string(unmodifiedStats.bestSeed)); printf(",");
	JSONPrint("best-time"); printf(":"); JSONPrint(unmodifiedStats.bestTime); printf(",");
	JSONPrint("worst-dps"); printf(":"); JSONPrint(unmodifiedStats.worstDPS); printf(",");
	JSONPrint("worst-seed"); printf(":"); JSONPrint(std::to_string(unmodifiedStats.worstSeed)); printf(",");
	JSONPrint("worst-time"); printf(":"); JSONPrint(unmodifiedStats.worstTime); printf(",");
	JSONPrint("effects"); printf(":"); JSONPrint(unmodifiedStats.effects); printf(",");
	
	const std::unordered_map<std::string, int*> scalingStats({
		{"wpdmg", &subjectConfiguration.stats.weaponPhysicalDamage},
		{"wmdmg", &subjectConfiguration.stats.weaponMagicDamage},
		{"str", &subjectConfiguration.stats.strength},
		{"dex", &subjectConfiguration.stats.dexterity},
		{"int", &subjectConfiguration.stats.intelligence},
		{"pie", &subjectConfiguration.stats.piety},
		{"crt", &subjectConfiguration.stats.criticalHitRate},
		{"det", &subjectConfiguration.stats.determination},
		{"sks", &subjectConfiguration.stats.skillSpeed},
		{"sps", &subjectConfiguration.stats.spellSpeed}
	});
	
	JSONPrint("scaling"); printf(":{");

	auto petConfiguration = subjectParser.petConfiguration();

	bool first = true;
	for (auto& kv : scalingStats) {
		if (!*kv.second) { continue; }
		
		auto original = *kv.second;
		auto amount = 5;

		// scale up
		*kv.second = original + amount;
		if (petConfiguration) {
			petConfiguration->stats = subjectConfiguration.stats;
		}
		
		// simulate
		SimulationStats scaleStats;
		PerformSimulations(100000, subjectConfigurations, &targetConfiguration, minLength, maxLength, &scaleStats);
		
		if (!first) { printf(","); }
		JSONPrint(kv.first); printf(":");
		JSONPrintDict(
			"amount", amount,
			"iterations", scaleStats.iterations,
			"time", scaleStats.time,
			"damage", scaleStats.general.damageDealt
		);
		first = false;

		// scale back down
		*kv.second = original;
		if (petConfiguration) {
			petConfiguration->stats = subjectConfiguration.stats;
		}
	}
	
	printf("}");

	printf("}");

	return 0;
}

}