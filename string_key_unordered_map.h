#pragma once

#include <string>
#include <string_view>
#include <unordered_map>

#include <tepp/cstring_view.h>

namespace te
{
	struct string_key_hasher
	{
		using is_transparent = void;

		size_t operator()(std::string_view key) const {
			return std::hash<std::string_view>()(key);
		}

		size_t operator()(std::string const& key) const {
			return std::hash<std::string_view>()(key);
		}

		size_t operator()(te::cstring_view key) const {
			return std::hash<std::string_view>()(key.string_view());
		}
	};

	template<class T>
	using string_key_unordered_map = std::unordered_map<std::string, T, string_key_hasher, std::equal_to<>>;

	template<class T>
	using string_key_unordered_multimap = std::unordered_multimap<std::string, T, string_key_hasher, std::equal_to<>>;
}
