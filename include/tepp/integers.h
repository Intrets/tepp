#pragma once

#include <cstddef>
#include <cstdint>

#include <tepp/safety_cast.h>

using integer_t = std::ptrdiff_t;
using qualifier_t = int64_t;

template<class T>
concept has_size_function = requires(T t) {
	{ t.size() } -> std::integral;
};

template<class I = integer_t, class T>
inline I isize(T const& a) {
	if constexpr (has_size_function<T>) {
		return static_cast<I>(a.size());
	}
	else {
		return static_cast<I>(a.size);
	}
}

constexpr inline integer_t operator""_i(unsigned long long i) {
	return static_cast<integer_t>(i);
}
