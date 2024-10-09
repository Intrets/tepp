#pragma once

#include "tepp/assert.h"

#include <utility>

namespace te
{
	template<class To, class From>
	To safety_cast(From const& from) {
		if (!std::in_range<To>(from)) {
			tassert(0);
			std::abort();
		}

		return static_cast<To>(from);
	}
}
