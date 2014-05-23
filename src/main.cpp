#include "Actor.h"
#include "Simulation.h"

#include "models/Monk.h"
#include "rotations/Monk.h"

using namespace std::literals::chrono_literals;

int main(int argc, const char* argv[]) {
	models::Monk subjectModel;
	rotations::Monk subjectRotation;
	Actor::Configuration subjectConfiguration;
	subjectConfiguration.stats.weaponDamage = 53;
	subjectConfiguration.stats.strength = 568;
	subjectConfiguration.stats.criticalHitRate = 469;
	subjectConfiguration.stats.skillSpeed = 425;
	subjectConfiguration.stats.determination = 332;
	subjectConfiguration.model = &subjectModel;
	subjectConfiguration.rotation = &subjectRotation;

	models::Monk targetModel;
	Actor::Configuration targetConfiguration;
	targetConfiguration.model = &targetModel;
	
	Simulation::Configuration simulationConfiguration;
	simulationConfiguration.length = 10min;
	simulationConfiguration.subjectConfiguration = &subjectConfiguration;
	simulationConfiguration.targetConfiguration = &targetConfiguration;
	Simulation simulation(&simulationConfiguration);
	simulation.run();
	
	auto& overall = simulation.stats();
	printf("OVERALL: %d damage done, %.3f dps\n\n", overall.totalDamageDealt, overall.totalDamageDealt / std::chrono::duration<double>(simulationConfiguration.length).count());
	
	printf("EFFECT                              DAMAGE         COUNT\n");
	
	auto& effects = simulation.statsByEffect();
	for (auto& kv : effects) {
		printf("%-28s  %12d  %12d\n", kv.first.c_str(), kv.second.totalDamageDealt, kv.second.count);
	}

	return 0;
}