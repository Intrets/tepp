#include "tepp/hash.h"

namespace te
{
	size_t hashBytes(std::span<std::byte> bytes) {
		// FNV-1
		constexpr uint64_t prime = 1099511628211ULL;
		constexpr uint64_t offset = 14695981039346656037ULL;

		uint64_t hash = offset;

		for (auto byte : bytes) {
			hash *= prime;
			hash ^= static_cast<uint64_t>(std::bit_cast<uint8_t>(byte));
		}

		return static_cast<size_t>(hash);
	}
}
