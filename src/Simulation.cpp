#include "Simulation.h"

#include "Action.h"
#include "Model.h"

using namespace std::literals::chrono_literals;

void Simulation::run() {
	Actor subject(_configuration->subjectConfiguration);
	Actor target(_configuration->targetConfiguration);

	auto action = subject.act(&target);
	auto damage = target.acceptedDamage(subject.generatedDamage(action));
	_results.totalDamageDealt += damage.amount;

	// TODO
}