#pragma once

#include <utility>

namespace te
{
	template<class To, class From>
	To safety_cast(From const& from) {
		if (!std::in_range<To>(from)) {
			std::abort();
		}

		return static_cast<To>(from);
	}
}
