#pragma once

#include "optional_ref.h"

namespace te
{
	template<class T>
	concept can_lookup_front = requires(T t) {
		{ t.empty() } -> std::same_as<bool>;
		t.front();
		typename std::remove_reference_t<T>::value_type;
	};

	template<can_lookup_front T>
	auto get_front(T&& t) {
		using S = std::remove_reference_t<T>::value_type;

		if (!t.empty()) {
			return te::optional_ref<S>(t.front());
		}
		else {
			return te::optional_ref<S>(te::nullopt);
		}
	}
}
