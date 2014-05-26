#include "../Actor.h"
#include "../JITRotation.h"
#include "../Simulation.h"

#include "../models/Monk.h"

#include <future>
#include <vector>

namespace applications {

struct TrialResults {
	Simulation::Configuration configuration;
	Actor::SimulationStats overallStats;
	std::unordered_map<std::string, Actor::EffectSimulationStats> effectStats;
};

static TrialResults Trial(const Simulation::Configuration&& configuration) {
	TrialResults ret;
	ret.configuration = configuration;

	Simulation simulation(&ret.configuration);
	simulation.run();

	ret.overallStats = simulation.subject()->simulationStats();
	ret.effectStats = simulation.subject()->effectSimulationStats();
	return ret;
}

int MultiIteration(int argc, const char* argv[]) {
	if (argc < 1) {
		printf("Rotation required.");
		return 1;
	}
	
	JITRotation subjectRotation;
	if (!subjectRotation.initializeWithFile(argv[0])) {
		printf("Unable to read rotation.\n");
		return 1;
	}
	
	models::Monk subjectModel;
	Actor::Configuration subjectConfiguration;
	subjectConfiguration.stats.weaponDamage = 47;
	subjectConfiguration.stats.weaponDelay = 2.72;
	subjectConfiguration.stats.strength = 512;
	subjectConfiguration.stats.criticalHitRate = 475;
	subjectConfiguration.stats.skillSpeed = 402;
	subjectConfiguration.stats.determination = 329;
	subjectConfiguration.model = &subjectModel;
	subjectConfiguration.rotation = &subjectRotation;

	models::Monk targetModel;
	Actor::Configuration targetConfiguration;
	targetConfiguration.model = &targetModel;
	
	int iterations = 3000;

	Actor::SimulationStats overallStats;
	std::map<std::string, Actor::EffectSimulationStats> effectStats;
	auto totalSimulationTime = 0_us;
	
	std::vector<std::future<TrialResults>> futures;

	std::random_device randomDevice;

	for (int i = 0; i < iterations; ++i) {
		Simulation::Configuration configuration;
		configuration.length = std::chrono::duration_cast<std::chrono::microseconds>(7_min + (i % 100) / 50.0 * 6_min);
		configuration.rng = &randomDevice;
		configuration.subjectConfiguration = &subjectConfiguration;
		configuration.targetConfiguration = &targetConfiguration;
		
		futures.emplace_back(std::async(Trial, std::move(configuration)));
	}
	
	for (auto& future : futures) {
		auto results = future.get();
		
		overallStats += results.overallStats;
		for (auto& kv : results.effectStats) {
			effectStats[kv.first] += kv.second;
		}
		totalSimulationTime += results.configuration.length;
	}

	{
		auto simulationTime = std::chrono::duration<double>(totalSimulationTime).count();

		printf("Iterations: %d\n", iterations);
		printf("Simulation Lengths: 7 - 13 minutes\n");
		printf("Total Simulated Time: %.2f minutes\n", simulationTime / 60.0);

		printf("\n");

		printf("Overall Damage Done: %d\n", overallStats.damageDealt);
		printf("Overall DPS: %.3f\n", overallStats.damageDealt / simulationTime);

		printf("\n");
		
		printf("EFFECT                              DAMAGE           DPS         COUNT         CRITS           AVG\n");
		for (auto& kv : effectStats) {
			auto& stats = kv.second;
			printf("%-28s  %12d  %12.3f  %12d  %12d  %12.3f\n", kv.first.c_str(), stats.damageDealt, stats.damageDealt / simulationTime, stats.count, stats.criticalHits, stats.damageDealt / (double)stats.count);
		}
	}

	return 0;
}

}