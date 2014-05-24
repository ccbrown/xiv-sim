#include "../Actor.h"
#include "../Simulation.h"

#include "../models/Monk.h"
#include "../rotations/Monk.h"

#include <future>
#include <vector>

namespace applications {

struct TrialResults {
	Simulation::Configuration configuration;
	Simulation::Stats overallStats;
	std::map<std::string, Simulation::Stats> statsByEffect;
};

static TrialResults Trial(const Simulation::Configuration&& configuration) {
	TrialResults ret;
	ret.configuration = configuration;

	Simulation simulation(&ret.configuration);
	simulation.run();

	ret.overallStats = simulation.stats();
	ret.statsByEffect = simulation.statsByEffect();
	return ret;
}

int MultiIteration(int argc, const char* argv[]) {
	models::Monk subjectModel;
	rotations::Monk subjectRotation;
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

	Simulation::Stats overallStats;
	std::map<std::string, Simulation::Stats> statsByEffect;
	auto totalSimulationTime = 0_us;
	
	std::vector<std::future<TrialResults>> futures;

	for (int i = 0; i < iterations; ++i) {
		Simulation::Configuration configuration;
		configuration.length = std::chrono::duration_cast<std::chrono::microseconds>(7_min + (i % 100) / 50.0 * 6_min);
		configuration.subjectConfiguration = &subjectConfiguration;
		configuration.targetConfiguration = &targetConfiguration;
		
		futures.emplace_back(std::async(Trial, std::move(configuration)));
	}
	
	for (auto& future : futures) {
		auto results = future.get();
		
		overallStats += results.overallStats;
		for (auto& kv : results.statsByEffect) {
			statsByEffect[kv.first] += kv.second;
		}
		totalSimulationTime += results.configuration.length;
	}

	{
		auto simulationTime = std::chrono::duration<double>(totalSimulationTime).count();

		printf("Iterations: %d\n", iterations);
		printf("Simulation Lengths: 7 - 13 minutes\n");
		printf("Total Simulated Time: %.2f minutes\n", simulationTime / 60.0);

		printf("\n");

		printf("Overall Damage Done: %d\n", overallStats.totalDamageDealt);
		printf("Overall DPS: %.3f\n", overallStats.totalDamageDealt / simulationTime);

		printf("\n");
		
		printf("EFFECT                              DAMAGE           DPS         COUNT         CRITS           AVG\n");
		for (auto& kv : statsByEffect) {
			auto& stats = kv.second;
			printf("%-28s  %12d  %12.3f  %12d  %12d  %12.3f\n", kv.first.c_str(), stats.totalDamageDealt, stats.totalDamageDealt / simulationTime, stats.count, stats.criticalHits, stats.totalDamageDealt / (double)stats.count);
		}
	}

	return 0;
}

}