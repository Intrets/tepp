#pragma once

#include <span>

namespace te
{
	template<class T>
	struct hash_struct
	{
		size_t operator()(T const& data) const {
			// FNV-1
			constexpr size_t prime = 1099511628211ULL;
			constexpr size_t offset = 14695981039346656037ULL;

			size_t hash = offset;

			auto constexpr byteSize = sizeof(T);
			std::byte const* raw = reinterpret_cast<std::byte const*>(&data);

			for (auto byte : std::span(raw, byteSize)) {
				hash *= prime;
				hash ^= static_cast<size_t>(std::bit_cast<uint8_t>(byte));
			}

			return hash;
		}
	};

	template<class T>
	size_t hash(T const& a) {
		return hash_struct<T>()(a);
	}
}
