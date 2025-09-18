#pragma once

#include "tepp/integers.h"

#include <algorithm>

namespace te
{
	namespace detail
	{
		template<class T>
		concept has_size = requires(T t) { isize(t); };
	}

	bool in_range(integer_t index, detail::has_size auto&& range) {
		return index >= 0 && index < isize(range);
	}

	integer_t clamp_in_range(integer_t index, detail::has_size auto&& range) {
		return std::clamp(index, 0_i, isize(range) - 1);
	}
}
