#pragma once

#include <chrono>
#include <string>

#include <stdint.h>

inline constexpr std::chrono::hours operator"" _h(unsigned long long n) {
	return std::chrono::hours(n);
}

inline constexpr std::chrono::minutes operator"" _min(unsigned long long n) {
	return std::chrono::minutes(n);
}

inline constexpr std::chrono::seconds operator"" _s(unsigned long long n) {
	return std::chrono::seconds(n);
}

inline constexpr std::chrono::milliseconds operator"" _ms(unsigned long long n) {
	return std::chrono::milliseconds(n);
}

inline constexpr std::chrono::microseconds operator"" _us(unsigned long long n) {
	return std::chrono::microseconds(n);
}

inline constexpr std::chrono::nanoseconds operator"" _ns(unsigned long long n) {
	return std::chrono::nanoseconds(n);
}

inline std::string operator"" _s(const char* str, size_t sz) {
	return std::string(str, sz);
}

inline uint64_t FNV1AHash(const char* str) {
	uint64_t ret = 14695981039346656037ull;
	for (auto ptr = str; *ptr; ++ptr) {
		ret = (ret ^ *(unsigned char*)ptr) * 1099511628211ull;
	}
	return ret;
}

inline uint64_t FNV1AHash(const std::string& str) {
	return FNV1AHash(str.c_str());
}