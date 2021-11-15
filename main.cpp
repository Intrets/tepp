#include <concepts>
#include <tuple>
#include <type_traits>
#include <ranges>

#include "tepp/tepp.h"

namespace detail
{
	template<class L>
	struct is_list;

	template<template<class...> class L, class... Args>
	struct is_list<L<Args...>> : std::true_type
	{
	};

	template<class L>
	struct is_list : std::false_type
	{
	};
}

template<class T>
concept list = detail::is_list<T>::value;

namespace detail
{
	template<list L>
	struct list_size;

	template<template<class...> class L, class... Args>
	struct list_size<L<Args...>>
	{
		static constexpr auto value = sizeof...(Args);
	};
}

template<list L>
constexpr auto list_size_v = detail::list_size<L>::value;

namespace detail
{
	template<list From, template<class...> class To>
	struct convert_list;

	template<template<class...> class From, class... Args, template<class...> class To>
	struct convert_list<From<Args...>, To>
	{
		using type = To<Args...>;
	};

	template<list L, class T>
	struct append;

	template<template<class...> class L, class... Args, class T>
	struct append<L<Args...>, T>
	{
		using type = L<Args..., T>;
	};

	template<list L, class T>
	struct prepend;

	template<template<class...> class L, class... Args, class T>
	struct prepend<L<Args...>, T>
	{
		using type = L<T, Args...>;
	};

	template<list L>
	struct head;

	template<template<class...> class L, class Arg, class... Args>
	struct head<L<Arg, Args...>>
	{
		using type = Arg;
	};
}

namespace detail
{
	template<list L>
	struct list_cat;

	template<template<list...> class L, list Arg>
	struct list_cat<L<Arg>>
	{
		using type = std::conditional_t<
			list_size_v<Arg> == 0,
			L<>,
			L<Arg>
		>;
	};

	template<template<list...> class L, list Arg, list... Args>
	struct list_cat<L<Arg, Args...>>
	{
		using next = typename detail::list_cat<L<Args...>>::type;

		using type = std::conditional_t<
			list_size_v<Arg> == 0,
			next,
			typename detail::prepend<next, Arg>::type
		>;
	};
}

template<list List>
using list_cat_t = typename detail::list_cat<List>::type;

namespace detail
{
	template<class T>
	struct Type
	{
		using type = T;
	};

	template<class T>
	struct is_type;

	template<class T>
	struct is_type<Type<T>> : std::true_type
	{
	};

	template<class T>
	struct is_type : std::false_type
	{
	};
}

template<class T>
using Type_t = detail::Type<T>;

template<class T>
constexpr auto Type = detail::Type<T>();

template<class T>
concept type = detail::is_type<T>::value;

namespace detail
{
	template<auto v>
	struct Value
	{
		using value_type = decltype(v);
		static constexpr value_type value = v;
	};

	template<class V>
	struct is_value;

	template<auto v>
	struct is_value<Value<v>> : std::true_type
	{
	};

	template<class V>
	struct is_value : std::false_type
	{
	};
}

template<auto v>
constexpr auto Value = detail::Value<v>();

template<auto v>
using Value_t = detail::Value<v>;

template<class V>
concept value = detail::is_value<V>::value;

template<class T>
concept meta = value<T> || type<T>;

template<auto... values>
constexpr auto value_list = Type<std::tuple<Value_t<values>...>>;

template<class... Args>
constexpr auto type_list = Type<std::tuple<detail::Type<Args>...>>;

template<meta T1, meta T2>
consteval auto operator== (T1, T2) {
	return Value<std::same_as<T1, T2>>;
}

namespace detail
{
	template<class F, list List>
	struct map;

	template<class F, template<meta...> class L, meta... Args>
	struct map<F, L<Args...>>
	{
		using type = L<std::invoke_result_t<F, Args>...>;
	};
}

template<class F, list L>
using map_t = detail::map<F, L>::type;

template<class F, list L>
constexpr auto map(F, Type_t<L>) {
	return Type<map_t<F, L>>;
}

namespace detail
{
	template<class F, list L>
	struct filter;

	template<class F, template<meta...> class L, meta Arg>
	struct filter<F, L<Arg>>
	{
		using type = std::conditional_t<
			std::invoke_result_t<F, Arg>::value,
			L<Arg>,
			L<>
		>;
	};

	template<class F, template<meta...> class L, meta Arg, meta... Args>
	struct filter<F, L<Arg, Args...>>
	{
		using next = typename detail::filter<F, L<Args...>>::type;

		using type = std::conditional_t<
			std::invoke_result_t<F, Arg>::value,
			typename detail::prepend<next, Arg>::type,
			next
		>;
	};
}

template<class F, list L>
using filter_t = typename detail::filter<F, L>::type;

template<class F, list L>
constexpr auto filter(F, Type_t<L>) {
	return Type<filter_t<F, L>>;
}

namespace detail
{
	template<class F, list L>
	struct all;

	template<class F, template<meta...> class L, meta... Args>
	struct all<F, L<Args...>>
	{
		constexpr static bool value = (std::invoke_result_t<F, Args>::value && ...);
	};
}

template<class F, list L>
constexpr bool all_v = detail::all<F, L>::value;

template<class F, list L>
constexpr auto all(F, Type_t<L>) {
	return Value<all_v<F, L>>;
}

constexpr auto id = [](auto x) {
	return x;
};

constexpr auto and_f = []<list L>(Type_t<L>) {
	return all(id, Type<L>);
};

namespace detail
{
	constexpr auto brute_compose_test(auto f1, auto f2) {
		return [=](auto x) {
			return f1(f2(x));
		};
	}
}

template<class F1, class F2>
concept can_compose = requires(F1 f1, F2 f2) {
	detail::brute_compose_test(f1, f2);
};

template<class F1, class F2>
	requires can_compose<F1, F2>
constexpr auto operator|(F1 f1, F2 f2) {
	return [=](auto x) {
		return f1(f2(x));
	};
}

namespace detail
{
	constexpr auto brute_apply_test(auto f, auto arg) {
		return f(arg);
	}
}

template<class F, class Arg>
concept can_apply = requires(F f, Arg arg) {
	detail::brute_apply_test(f, arg);
};

template<class F, class Arg>
	requires can_apply<F, Arg>
constexpr auto operator&&(F f, Arg arg) {
	return f(arg);
}

template<class... Args>
struct Ok
{
	std::tuple<Args...> data;

	Ok(Args... args) {

	}
};

//template<class... Args>
//Ok(Args...)->Ok<Args...>;

int main() {
	//constexpr std::tuple<detail::Type<float>, Type<int>> t;

	constexpr auto l = type_list<float, int, float>;

	//constexpr auto l = type_list<Value<1>, Value<2>, Value<3>>;
	constexpr int r = 1;
	constexpr auto l2 = value_list<1, 2, 3>;


	constexpr auto map_test = map(
		[]<auto v>(Value_t<v>) {
		return Value<v * 2>;
	},
		l2);

	constexpr auto map_test_types = map(
		[]<class T>(T x) {
		if constexpr (std::same_as<int, decltype(x)::type>) {
			return Value<1>;
		}
		else {
			return Type<decltype(x)::type>;
		}
	},
		l);

	rand();



	constexpr auto b = list<std::tuple<int>>;
	constexpr auto b2 = list<int>;

	//constexpr auto aaaaaaaaaaaaa = detail::list_cat<>;


	constexpr auto l5 = value_list<1, 2, 3, 4, 5, 6>;


	constexpr auto f = []<type T>(T t) {
		return std::same_as<typename T::type, float>;
	};


	constexpr auto g = []<auto v>(Value_t<v>) {
		//return Value<v % 2 == 0>;
		return v % 2 == 0;
	};

	constexpr auto g1 = []<auto v>(Value_t<v>) {
		return Value<v + 1>;
	};

	constexpr auto simple = [](auto x) {
		return 1;
	};

	constexpr auto test_lambda = []<auto v>(Value_t<v>) {
		return[=]<auto x>(Value_t<x>) {
			return Value<x% v == 0>;
		};
	};

	constexpr auto broke = test_lambda(Value<2>);
	constexpr auto not_broke1 = broke(Value<3>);
	constexpr auto not_broke2 = broke(Value<4>);

	constexpr auto gg = g1 | g1;
	constexpr auto ggg = gg | g1;
	constexpr auto gggg = g1 | ggg;

	constexpr auto test = (g | ggg)(Value<1>);
	constexpr auto test2 = ggg && Value<2>;

	//std::ranges;
	auto constexpr ok = std::views::drop(1);

	//constexpr auto zz = filter(g | 1, l5);

	constexpr int test1 = 1 | 2 | 3 && 1;


	constexpr auto even = []<auto v>(Value_t<v>) {
		return Value<v % 2 == 0>;
	};

	constexpr auto testing_all = all(even, value_list<1, 4, 6>);
	constexpr auto testing_and1 = and_f(value_list<true, true, false>);
	constexpr auto testing_and2 = and_f(value_list<true, true, true>);

	Ok(1, 2, 3);


	rand();

	//g(Wildcard);


	//constexpr bool t1 = te::number_of_arguments_is<decltype(g), 1>;
	////constexpr bool t2 = te::number_of_arguments_is<decltype(simple), 1>;
	////constexpr bool t3 = te::number_of_arguments_is<int, 1>;

	//constexpr bool t3 = is_function_like<decltype(simple)>;
	//static_assert(t3);

	//constexpr bool t4 = is_function_like<decltype(g)>;
	//static_assert(t4);

	//constexpr bool t5 = is_function_like<int>;
	//static_assert(!t5);

	//constexpr bool t5 = has_value_argument<decltype(simple)>;
	//constexpr bool t6 = has_value_argument<int>;

	//auto test = g(Value<1.0f>);


	rand();
	return 0;
}