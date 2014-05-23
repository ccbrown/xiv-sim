#pragma once

#include "Actor.h"

#include <chrono>

class Simulation {
	public:
		struct Configuration {
			std::chrono::milliseconds length;
			Actor::Configuration* subjectConfiguration = nullptr;
			Actor::Configuration* targetConfiguration = nullptr;
		};
		
		struct Results {
			unsigned int totalDamageDealt = 0;
		};
	
		Simulation(const Configuration* configuration) : _configuration(configuration) {}

		void run();
		
		const Results& results() const { return _results; }

	private:
		const Configuration* const _configuration = nullptr;
		
		Results _results;
};