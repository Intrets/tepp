#pragma once

#include <concepts>
#include <cstddef>
#include <cstdint>
#include <utility>

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

template<std::integral I>
inline constexpr static I imod(I i, I m) {
	auto i2 = i % m;
	return i2 >= 0 ? i2 : m + i2;
}

template<std::integral I>
inline constexpr static std::pair<I, I> idivmod(I i, I m) {
	auto i2 = i % m;
	auto i3 = i / m;
	return { i3 - I(i2 < I(0)), i2 < I(0) ? i2 + m : i2 };
}

static_assert(idivmod(0, 3) == std::pair(0, 0));

static_assert(idivmod(14, 3) == std::pair(4, 2));
static_assert(idivmod(13, 3) == std::pair(4, 1));
static_assert(idivmod(12, 3) == std::pair(4, 0));
static_assert(idivmod(11, 3) == std::pair(3, 2));
static_assert(idivmod(10, 3) == std::pair(3, 1));

static_assert(idivmod(-10, 3) == std::pair(-4, 2));
static_assert(idivmod(-11, 3) == std::pair(-4, 1));
static_assert(idivmod(-12, 3) == std::pair(-4, 0));
static_assert(idivmod(-13, 3) == std::pair(-5, 2));
static_assert(idivmod(-14, 3) == std::pair(-5, 1));
