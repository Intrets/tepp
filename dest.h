// tepp - A template library for C++
// Copyright (C) 2021 intrets

#pragma once

#include <utility>
#include <type_traits>

#include "tepp.h"

#define UNPACK(n, ...)\
template<class T>\
static auto dest##n() {\
	auto [__VA_ARGS__] = T();\
\
	return decltype(convert(__VA_ARGS__))();\
}\
\
template<class T>\
requires (!std::is_empty_v<T> && has_n_members<T, n>::value) \
struct Dest<T>\
{\
	using type = decltype(dest##n<T>());\
};\
\
template<class T>\
requires (!std::is_empty_v<T> && has_n_members<T, n>::value) \
static auto get_tie(T& t)\
{\
	auto& [__VA_ARGS__] = t;\
\
	return std::tie(__VA_ARGS__);\
}

namespace te
{
	template<class... T>
	static auto convert(T... es) {
		return te::list_type<std::remove_cvref_t<T>...>();
	}

	template<class T>
	struct wrap
	{
		using type = T;
	};

	struct WildCard
	{
		template<class T>
		operator T() const;
	};

	namespace detail
	{
		namespace detail
		{
			template<class T, class... S>
			concept has_members = requires (T t) { t = { std::declval<S>()... }; };
		}

		template<class T, list L>
		struct has_members;

		template<class T, template<class...>class L, class... Args>
		struct has_members<T, L<Args...>>
		{
			constexpr static bool value = detail::has_members<T, Args...>;
		};
	}

	template<int N, class T>
	concept has_annotated_count = requires (T) { T::member_count; } && T::member_count == N;

	constexpr auto has_atleast_n_members = []<int N, class T>(te::Value_t<N>, te::Type_t<T>) {
		return Value<detail::has_members<T, te::replicate_t<N, WildCard>>::value>;
	};

	constexpr auto has_n_members_f = []<class T, int N>(te::Type_t<T>, te::Value_t<N>) {
		constexpr bool is_annotated = has_annotated_count<N, T>;
		constexpr auto has_n = (has_atleast_n_members(te::Value<N>, te::Type<T>) &&
			!has_atleast_n_members(te::Value<N + 1>, te::Type<T>));
		return Value<is_annotated> || has_n;
	};

	template<class T, int N>
	constexpr static bool has_n_members_v = Getvalue(has_n_members_f(te::Type<T>, te::Value<N>));

	template<class T, int N>
	struct has_n_members
	{
		constexpr static bool value = has_n_members_v<T, N>;
	};

	template<class T>
	struct Dest;

	template<class T>
		requires std::is_empty_v<T>
	struct Dest<T>
	{
		using type = list_type<>;
	};

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

	template<class T>
	using get_members_t = typename Dest<T>::type;

	constexpr auto get_members = []<class T>(Type_t<T>) {
		return Type<get_members_t<T>>;
	};
}
