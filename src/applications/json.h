#pragma once

#include "../Actor.h"

class Action;

#include <chrono>
#include <map>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include <stdio.h>
#include <inttypes.h>
#include <stdint.h>

namespace applications {

void JSONPrint(const char* string);
void JSONPrint(const std::string& string);
void JSONPrint(int32_t n);
void JSONPrint(int64_t n);
void JSONPrint(uint64_t n);
void JSONPrint(double f);
void JSONPrint(const Actor::Stats& actorStats);
void JSONPrint(const Actor::EffectSimulationStats& effectStats);
void JSONPrint(const Action* action);

template <typename Rep, typename Period>
void JSONPrint(const std::chrono::duration<Rep, Period>& d) {
	JSONPrint(std::chrono::duration_cast<std::chrono::microseconds>(d).count());
}

template <typename T>
void JSONPrint(const std::vector<T>& vector);

template <typename T, typename U>
void JSONPrint(const std::pair<T, U>& pair) {
	printf("[");
	JSONPrint(pair.first);
	printf(",");
	JSONPrint(pair.second);
	printf("]");
}

template <typename Key, typename Value>
void JSONPrint(const std::map<Key, Value>& map) {
	printf("{");
	bool first = true;
	for (auto& kv : map) {
		if (!first) { printf(","); }
		JSONPrint(kv.first); printf(":"); JSONPrint(kv.second);
		first = false;
	}
	printf("}");
}

template <typename Key, typename Value>
void JSONPrint(const std::unordered_map<Key, Value>& map) {
	printf("{");
	bool first = true;
	for (auto& kv : map) {
		if (!first) { printf(","); }
		JSONPrint(kv.first); printf(":"); JSONPrint(kv.second);
		first = false;
	}
	printf("}");
}

template <typename T>
void JSONPrint(const std::vector<T>& vector) {
	printf("[");
	bool first = true;
	for (auto& obj : vector) {
		if (!first) { printf(","); }
		JSONPrint(obj);
		first = false;
	}
	printf("]");
}

inline void JSONPrintRemainingDictPairs() {}

template <typename Value, typename... Rem>
void JSONPrintRemainingDictPairs(const char* key, Value&& value, Rem&&... rem) {
	printf(",");
	JSONPrint(key);
	printf(":");
	JSONPrint(std::forward<Value>(value));
	JSONPrintRemainingDictPairs(std::forward<Rem>(rem)...);
}

template <typename Value, typename... Rem>
void JSONPrintDict(const char* key, Value&& value, Rem&&... rem) {
	printf("{");
	JSONPrint(key);
	printf(":");
	JSONPrint(std::forward<Value>(value));
	JSONPrintRemainingDictPairs(std::forward<Rem>(rem)...);
	printf("}");
}

} // namespace applications