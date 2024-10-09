#pragma once

#include <cstddef>
#include <cstdint>

#include <tepp/safety_cast.h>

using integer_t = std::ptrdiff_t;
using qualifier_t = int64_t;

template<class I = integer_t>
inline I isize(auto&& a) {
	return static_cast<I>(a.size());
}

constexpr inline integer_t operator""_i(unsigned long long i) {
	return static_cast<integer_t>(i);
}
