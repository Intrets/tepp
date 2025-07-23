#pragma once

#include <unordered_map>

#include <tepp/optional_ref.h>

namespace te
{
	template<class T, class S>
	auto lookup(T& map, S const& value) {
		using R = optional_ref<typename T::mapped_type>;

		auto it = map.find(value);
		if (it == map.end()) {
			return R(te::nullopt);
		}
		else {
			return R(it->second);
		}
	}

	template<class T, class S>
	auto lookup(T const& map, S const& value) {
		using R = optional_ref<typename T::mapped_type const>;

		auto it = map.find(value);
		if (it == map.end()) {
			return R(te::nullopt);
		}
		else {
			return R(it->second);
		}
	}
}
