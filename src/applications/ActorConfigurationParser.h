#pragma once

#include "../Actor.h"
#include "../Model.h"

#include <memory>

class ActorConfigurationParser {
	public:
		bool parseFile(const char* filename);
		bool parse(const char* str, size_t length);
		
		Actor::Configuration& configuration() { return _configuration; }
		Actor::Configuration* petConfiguration() { return _petConfiguration.get(); }
		
	private:
		Actor::Configuration _configuration;

		std::unique_ptr<Model> _model;
		std::unique_ptr<Model> _petModel;
		std::unique_ptr<Rotation> _petRotation;
		std::unique_ptr<Actor::Configuration> _petConfiguration;
};
