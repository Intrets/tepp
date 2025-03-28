#pragma once

#include <bit>
#include <span>
#include <type_traits>

#include <tepp/tepp.h>

namespace te
{
	namespace detail
	{
		template<class T>
		concept no_padding = std::has_unique_object_representations_v<T>;
	}

	size_t hashBytes(std::span<std::byte> bytes);

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

	struct hasher
	{
		// FNV-1
		static constexpr uint64_t prime = 1099511628211ULL;
		static constexpr uint64_t offset = 14695981039346656037ULL;

		uint64_t hash = offset;

		operator size_t() const {
			return static_cast<size_t>(this->hash);
		}

		template<detail::no_padding T>
		hasher& add(std::span<T> data) {
			for (auto byte : std::as_bytes(data)) {
				hash *= prime;
				hash ^= static_cast<uint64_t>(std::bit_cast<uint8_t>(byte));
			}

			return *this;
		}

		template<detail::no_padding... T>
		hasher& add(T const&... data) {
			te::for_each(
			    [&](auto const& data) {
				    auto constexpr byteSize = sizeof(data);
				    std::byte const* raw = reinterpret_cast<std::byte const*>(&data);

				    for (auto byte : std::span<std::byte const>(raw, byteSize)) {
					    this->hash *= prime;
					    this->hash ^= static_cast<uint64_t>(std::bit_cast<uint8_t>(byte));
				    }
			    },
			    data...
			);

			return *this;
		}
	};

	template<detail::no_padding... T>
	size_t hash(T const&... a) {
		return hash_struct<T...>()(a...);
	}
}
