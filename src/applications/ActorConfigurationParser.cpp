#include "ActorConfigurationParser.h"

#include "../PetRotation.h"

#include "../models/Bard.h"
#include "../models/BlackMage.h"
#include "../models/Dragoon.h"
#include "../models/Garuda.h"
#include "../models/Monk.h"
#include "../models/Summoner.h"

#include <regex>

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
				const std::regex expression("^\\s*([^=]+?)\\s*=\\s*(.+?)\\s*$");
				std::match_results<const char*> matches;
				std::regex_match(line, ptr, matches, expression);
				
				if (matches.size() == 3) {
					if (matches[1].str() == "weapon physical damage") {
						_configuration.stats.weaponPhysicalDamage = std::stoi(matches[2].str());
					} else if (matches[1].str() == "weapon magic damage") {
						_configuration.stats.weaponMagicDamage = std::stoi(matches[2].str());
					} else if (matches[1].str() == "weapon delay") {
						_configuration.stats.weaponDelay = std::stod(matches[2].str());
						if (_configuration.stats.weaponDelay < 0.3) {
							printf("Weapon delay out of bounds.\n");
							return false;
						}
					} else if (matches[1].str() == "strength") {
						_configuration.stats.strength = std::stoi(matches[2].str());
					} else if (matches[1].str() == "dexterity") {
						_configuration.stats.dexterity = std::stoi(matches[2].str());
					} else if (matches[1].str() == "intelligence") {
						_configuration.stats.intelligence = std::stoi(matches[2].str());
					} else if (matches[1].str() == "piety") {
						_configuration.stats.piety = std::stoi(matches[2].str());
					} else if (matches[1].str() == "critical hit rate") {
						_configuration.stats.criticalHitRate = std::stoi(matches[2].str());
					} else if (matches[1].str() == "determination") {
						_configuration.stats.determination = std::stoi(matches[2].str());
					} else if (matches[1].str() == "skill speed") {
						_configuration.stats.skillSpeed = std::stoi(matches[2].str());
					} else if (matches[1].str() == "spell speed") {
						_configuration.stats.spellSpeed = std::stoi(matches[2].str());
					} else if (matches[1].str() == "model") {
						if (matches[2].str() == "monk") {
							_model.reset(new models::Monk());
						} else if (matches[2].str() == "dragoon") {
							_model.reset(new models::Dragoon());
						} else if (matches[2].str() == "black-mage") {
							_model.reset(new models::BlackMage());
						} else if (matches[2].str() == "bard") {
							_model.reset(new models::Bard());
						} else if (matches[2].str() == "summoner") {
							_model.reset(new models::Summoner());
						} else {
							printf("Unknown model.\n");
							return false;
						}
						_configuration.model = _model.get();
					} else if (matches[1].str() == "pet") {
						if (matches[2].str() == "garuda") {
							_petModel.reset(new models::Garuda());
							_petRotation.reset(new PetRotation(_petModel->action("wind-blade")));
						} else {
							printf("Unknown pet.\n");
							return false;
						}
						_petConfiguration.reset(new Actor::Configuration());
						_petConfiguration->identifier = matches[2].str();
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