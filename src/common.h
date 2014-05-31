#pragma once

#include <cassert>
#include <chrono>
#include <string>
#include <type_traits>

#include <stdint.h>
#include <math.h>

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

template <typename IntType = int>
struct PortableUniformIntDistribution {
	explicit PortableUniformIntDistribution(IntType a = 0, IntType b = std::numeric_limits<IntType>::max()) : a(a), b(b) {}
		
	IntType min() const { return a; }
	IntType max() const { return b; }

	template <class URNG>
	IntType operator()(URNG& g) {
		auto range = b - a + 1;
		int bits = range ? (int)ceil(log2((double)range)) : (int)(sizeof(IntType) * 8);

		static const int bitsPerRandomNumber = sizeof(decltype(g())) * 8;

		typedef typename std::remove_reference<decltype(g)>::type GeneratorType;

		assert(GeneratorType::min() == 0);
		assert(GeneratorType::max() == (1ULL << bitsPerRandomNumber) - 1);

		while (true) {
			IntType result = 0;
			int needed = bits;
			while (needed >= bitsPerRandomNumber) {
				if (bitsPerRandomNumber < sizeof(result) * 8) {
					result <<= bitsPerRandomNumber;
				}
				result |= g();
				needed -= bitsPerRandomNumber;
			}
			if (needed) {
				result <<= needed;
				result |= (g() & ((1 << needed) - 1));
			}
			if (result <= range || !range) {
				return a + result;
			}
		}

		return a + g() % (b - a + 1);
	}
		
	IntType a, b;
};