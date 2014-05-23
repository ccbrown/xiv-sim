#include "Monk.h"

#include "../Action.h"
#include "../Actor.h"

#include <random>

namespace models {

Damage Monk::generatedDamage(const Action* action, const Actor* actor) const {
	auto& stats = actor->stats();
	
	Damage ret;
	double criticalHitChance = action->criticalHitChance((stats.criticalHitRate * 0.0697 - 18.437) / 100.0);

	std::random_device generator;
	std::uniform_real_distribution<double> distribution(0.0, 1.0);

	ret.isCritical = (distribution(generator) < criticalHitChance);

	ret.amount = action->damage() / 100.0 * (stats.weaponDamage * (stats.strength * 0.00389 + stats.determination * 0.0008 + 0.01035) + (stats.strength * 0.08034) + (stats.determination * 0.02622));

	// TODO: this sway is a complete guess off the top of my head and should be researched
	ret.amount *= 1.0 + (0.5 - distribution(generator)) * 0.1;

	if (ret.isCritical) {
		ret.amount *= 1.5;
	}

	return ret;
}

Damage Monk::acceptedDamage(const Damage& incoming, const Actor* actor) const {
	return incoming;
}

std::chrono::microseconds Monk::globalCooldown(const Actor* actor) const {
	auto& stats = actor->stats();

	auto gcd = std::chrono::duration<double>(2.49 - (stats.skillSpeed - 344) * (0.01 / 10.5));

	return std::chrono::duration_cast<std::chrono::microseconds>(gcd);
}

}