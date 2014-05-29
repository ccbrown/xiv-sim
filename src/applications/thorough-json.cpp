#include "ActorConfigurationParser.h"

#include "../Actor.h"
#include "../JITRotation.h"
#include "../Simulation.h"

#include "../models/Monk.h"

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
		std::uniform_int_distribution<uint64_t> dist;
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
	uint64_t bestSeed = 0;
	double bestDPS = 0.0;
	std::chrono::microseconds time = 0_us;
	Actor::SimulationStats general;
	std::map<std::string, Actor::EffectSimulationStats> effects;
};

void PerformSimulations(int iterations, Actor::Configuration* subjectConfiguration, Actor::Configuration* targetConfiguration, SimulationStats* stats) {
	stats->iterations = iterations;
	
	std::vector<std::future<TrialResults>> futures;

	for (int i = 0; i < iterations; ++i) {
		Simulation::Configuration configuration;
		configuration.length = std::chrono::duration_cast<std::chrono::microseconds>(7_min + (i % 100) / 50.0 * 6_min);
		configuration.subjectConfiguration = subjectConfiguration;
		configuration.targetConfiguration = targetConfiguration;
		
		futures.emplace_back(std::async(Trial, std::move(configuration)));
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
		}
		if (!stats->bestDPS || dps > stats->bestDPS) {
			stats->bestSeed = results.seed;
			stats->bestDPS = dps;
		}
	}
}

int ThoroughJSON(int argc, const char* argv[]) {
	if (argc < 2) {
		printf("Usage: simulator single-json subject rotation\n");
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

	if (subjectConfiguration.petConfiguration) {
		auto petConfiguration = *subjectConfiguration.petConfiguration;
		petConfiguration.identifier = "player-pet";
		petConfiguration.keepsHistory = subjectConfiguration.keepsHistory;
	}

	models::Monk targetModel;
	Actor::Configuration targetConfiguration;
	targetConfiguration.identifier = "target";
	targetConfiguration.model = &targetModel;

	SimulationStats unmodifiedStats;
	PerformSimulations(3000, &subjectConfiguration, &targetConfiguration, &unmodifiedStats);

	{
		auto simulationTime = std::chrono::duration<double>(unmodifiedStats.time).count();

		printf("Iterations: %d\n", unmodifiedStats.iterations);

		printf("\n");

		printf("Average DPS: %.3f\n", unmodifiedStats.general.damageDealt / simulationTime);
		printf("Worst DPS: %.3f (seed = %" PRIu64 ")\n", unmodifiedStats.worstDPS, unmodifiedStats.worstSeed);
		printf("Best DPS: %.3f (seed = %" PRIu64 ")\n", unmodifiedStats.bestDPS, unmodifiedStats.bestSeed);

		printf("\n");
		
		printf("EFFECT                              DAMAGE           DPS         COUNT         CRITS           AVG\n");
		for (auto& kv : unmodifiedStats.effects) {
			auto& stats = kv.second;
			printf("%-28s  %12d  %12.3f  %12d  %12d  %12.3f\n", kv.first.c_str(), stats.damageDealt, stats.damageDealt / simulationTime, stats.count, stats.criticalHits, stats.damageDealt / (double)stats.count);
		}
	}

	return 0;
}

}