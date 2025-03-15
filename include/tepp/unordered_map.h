#pragma once

#include <unordered_map>

#include <tepp/optional_ref.h>

namespace te
{
	template<class K, class T>
	te::optional_ref<T const> lookup(std::unordered_map<K, T> const& map, K const& value) {
		auto it = map.find(value);
		if (it == map.end()) {
			return te::nullopt;
		}
		else {
			return it->second;
		}
	}
}
