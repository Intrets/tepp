#pragma once

#include <bit>
#include <span>

#include <tepp/tepp.h>

namespace te
{
	namespace detail
	{
		template<class T>
		concept no_padding = std::has_unique_object_representations_v<T>;
	}

	template<detail::no_padding... T>
	struct hash_struct
	{
		size_t operator()(T const&... data) const {
			// FNV-1
			constexpr uint64_t prime = 1099511628211ULL;
			constexpr uint64_t offset = 14695981039346656037ULL;

			uint64_t hash = offset;

			te::for_each(
			    [&hash](auto const& data) {
				    auto constexpr byteSize = sizeof(data);
				    std::byte const* raw = reinterpret_cast<std::byte const*>(&data);

				    for (auto byte : std::span<std::byte const>(raw, byteSize)) {
					    hash *= prime;
					    hash ^= static_cast<uint64_t>(std::bit_cast<uint8_t>(byte));
				    }
			    },
			    data...
			);

			return static_cast<size_t>(hash);
		}
	};

	template<detail::no_padding... T>
	size_t hash(T const&... a) {
		return hash_struct<T...>()(a...);
	}
}
