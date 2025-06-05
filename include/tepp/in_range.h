#pragma once

#include "tepp/integers.h"

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
}
