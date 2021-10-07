#pragma once

#include <utility>

#include "tepp.h"

#define UNPACK(n, ...)\
template<class T>\
static auto dest##n() {\
	auto& [__VA_ARGS__] = std::declval<T>();\
\
	return std::declval<decltype(convert(__VA_ARGS__))>();\
}\
\
template<class T>\
requires has_n_members<T, n>::value \
struct Dest<T>\
{\
	using type = decltype(dest##n<T>());\
};

namespace te
{
	template<class... T>
	static auto convert(T... es) {
		return std::declval<te::list<std::remove_cvref_t<T>...>>();
	}

	template<class T>
	struct wrap
	{
		using type = T;
	};

	struct WildCard
	{
		template<class T>
		operator T () const {
			return T();
		};

		template<class T>
		operator T& () const {
			T a;
			return a;
		};

		template<class T>
		operator T && () const {
			T a;
			return a;
		};
	};

	template<class T, class... S>
	concept has_members = requires (T) { T(std::declval<S>()...); };

	template<class T, class L>
	struct has_members_t;

	template<class T, class... S>
	struct has_members_t<T, te::list<S...>>
	{
		constexpr static bool value = has_members<T, S...>;
	};

	template<class T, int N>
	struct has_n_members
	{
		constexpr static bool value = has_members_t<T, te::replicate_t<N, WildCard>>::value && !has_members_t<T, te::replicate_t<N + 1, WildCard>>::value;
	};

	template<class T>
	struct Dest;

	UNPACK(1, x1);
	UNPACK(2, x1, x2);
	UNPACK(3, x1, x2, x3);
	UNPACK(4, x1, x2, x3, x4);
	UNPACK(5, x1, x2, x3, x4, x5);
	UNPACK(6, x1, x2, x3, x4, x5, x6);
	UNPACK(7, x1, x2, x3, x4, x5, x6, x7);
	UNPACK(8, x1, x2, x3, x4, x5, x6, x7, x8);
	UNPACK(9, x1, x2, x3, x4, x5, x6, x7, x8, x9);
	UNPACK(10, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10);
	UNPACK(11, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11);
	UNPACK(12, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12);
	UNPACK(13, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13);
	UNPACK(14, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14);
}
