#pragma once

#include "tepp/integers.h"

#include "tepp/nullopt.h"
#include "tepp/optional_ref.h"
#include <algorithm>
#include <type_traits>

namespace te
{
	namespace detail
	{
		template<class T>
		concept has_size = requires(T t) { isize(t); };

		template<class T, class I>
		concept has_in_range = requires(T t, I i) {
			{ t.in_range(i) } -> std::same_as<bool>;
		};
	}

	bool in_range(integer_t index, detail::has_size auto&& range) {
		return index >= 0 && index < isize(range);
	}

	template<class I>
	bool in_range(I index, detail::has_in_range<I> auto&& range) {
		return range.in_range(index);
	}

	integer_t clamp_in_range(integer_t index, detail::has_size auto&& range) {
		return std::clamp(index, 0_i, isize(range) - 1);
	}

	auto range_at(integer_t i, detail::has_size auto&& range) {
		using T = std::remove_reference_t<decltype(range[i])>;
		using R = optional_ref<T>;

		if (in_range(i, range)) {
			return R(range[i]);
		}
		else {
			return R(nullopt);
		}
	}
}
