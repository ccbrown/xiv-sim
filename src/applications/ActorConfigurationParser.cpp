#include "ActorConfigurationParser.h"

#include "../PetRotation.h"

#include "../models/Bard.h"
#include "../models/BlackMage.h"
#include "../models/Dragoon.h"
#include "../models/Garuda.h"
#include "../models/Monk.h"
#include "../models/Summoner.h"

#include <cctype>

bool ActorConfigurationParser::parseFile(const char* filename) {
	FILE* f = fopen(filename, "r");

	if (!f) {
		printf("Unable to open file %s\n", filename);
		return false;
	}

	fseek(f, 0, SEEK_END);
	auto fsize = ftell(f);
	rewind(f);
 
	auto contents = (char*)malloc(fsize);
	if (!contents) {
		printf("Unable to allocate memory for configuration contents\n");
		fclose(f);
		return false;
	}

	if (fread(contents, fsize, 1, f) != 1) {
		printf("Unable to read file %s\n", filename);
		fclose(f);
		return false;
	}

	fclose(f);
	
	auto ret = parse(contents, fsize);
	
	free(contents);
	
	return ret;
}

bool ActorConfigurationParser::parse(const char* str, size_t length) {
	try {
		auto ptr = str;
		auto end = str + length;
		
		auto line = ptr;
		while (true) {
			if (ptr == end || *ptr == '\n') {
				std::string key;
				std::string value;
				
				if (auto assignment = (const char*)memchr(line, '=', ptr - line)) {
					while (isspace(*line)) { ++line; }
					key.assign(line, assignment - line);
					while (!key.empty() && isspace(key.back())) { key.pop_back(); }

					auto start = assignment + 1;
					while (start < ptr && isspace(*start)) { ++start; }
					value.assign(start, ptr - start);
					while (!value.empty() && isspace(value.back())) { value.pop_back(); }
				}
				
				if (!key.empty()) {
					if (key == "weapon physical damage") {
						_configuration.stats.weaponPhysicalDamage = std::stoi(value);
					} else if (key == "weapon magic damage") {
						_configuration.stats.weaponMagicDamage = std::stoi(value);
					} else if (key == "weapon delay") {
						_configuration.stats.weaponDelay = std::stod(value);
						if (_configuration.stats.weaponDelay < 0.3) {
							printf("Weapon delay out of bounds.\n");
							return false;
						}
					} else if (key == "strength") {
						_configuration.stats.strength = std::stoi(value);
					} else if (key == "dexterity") {
						_configuration.stats.dexterity = std::stoi(value);
					} else if (key == "intelligence") {
						_configuration.stats.intelligence = std::stoi(value);
					} else if (key == "piety") {
						_configuration.stats.piety = std::stoi(value);
					} else if (key == "critical hit rate") {
						_configuration.stats.criticalHitRate = std::stoi(value);
					} else if (key == "determination") {
						_configuration.stats.determination = std::stoi(value);
					} else if (key == "skill speed") {
						_configuration.stats.skillSpeed = std::stoi(value);
					} else if (key == "spell speed") {
						_configuration.stats.spellSpeed = std::stoi(value);
					} else if (key == "model") {
						if (value == "monk") {
							_model.reset(new models::Monk());
						} else if (value == "dragoon") {
							_model.reset(new models::Dragoon());
						} else if (value == "black-mage") {
							_model.reset(new models::BlackMage());
						} else if (value == "bard") {
							_model.reset(new models::Bard());
						} else if (value == "summoner") {
							_model.reset(new models::Summoner());
						} else {
							printf("Unknown model.\n");
							return false;
						}
						_configuration.model = _model.get();
					} else if (key == "pet") {
						if (value == "garuda") {
							_petModel.reset(new models::Garuda());
							_petRotation.reset(new PetRotation(_petModel->action("wind-blade")));
						} else {
							printf("Unknown pet.\n");
							return false;
						}
						_petConfiguration.reset(new Actor::Configuration());
						_petConfiguration->identifier = value;
						_petConfiguration->model = _petModel.get();
						_petConfiguration->rotation = _petRotation.get();
						_configuration.petConfiguration = _petConfiguration.get();
					}
				}
				
				if (ptr == end) { break; }
				line = ptr + 1;
			}
			++ptr;
		}
	} catch (...) {
		return false;
	}
	
	if (_petConfiguration) {
		_petConfiguration->stats = _configuration.stats;
	}
	
	return _model.get();
}