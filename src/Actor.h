#pragma once

#include "Damage.h"

class Action;
class Model;
class Rotation;

class Actor {
	public:
		struct Stats {
			int weaponDamage = 0;
			int strength = 0;
			int determination = 0;
			int criticalHitRate = 0;
			int skillSpeed = 0;
		};
	
		struct Configuration {
			Stats stats;
			const Model* model = nullptr;
			const Rotation* rotation = nullptr;
		};

		Actor(const Configuration* configuration) : _configuration(configuration), _stats(configuration->stats) {}

		const Action* act(const Actor* target) const;

		const Stats& stats() const { return _stats; }

		Damage generatedDamage(const Action* action) const;
		Damage acceptedDamage(const Damage& incoming) const;

	private:
		const Configuration* const _configuration = nullptr;
		Stats _stats;
};