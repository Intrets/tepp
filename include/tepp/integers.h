#pragma once

#include <cstddef>
#include <cstdint>

using integer_t = std::ptrdiff_t;
using qualifier_t = int64_t;

inline integer_t isize(auto&& a) {
	return static_cast<integer_t>(a.size());
}
