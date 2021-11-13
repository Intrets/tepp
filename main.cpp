#include <concepts>
#include <tuple>
#include <type_traits>

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
		using next = detail::list_cat<L<Args...>>::type;

		using type = std::conditional_t<
			list_size_v<Arg> == 0,
			next,
			typename detail::prepend<next, Arg>::type
		>;
	};
}

template<list List>
using list_cat_t = detail::list_cat<List>::type;

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
constexpr auto value_list = Type<std::tuple<detail::Value<values>...>>;

template<class... Args>
constexpr auto type_list = Type<std::tuple<detail::Type<Args>...>>;

consteval auto operator== (meta auto t1, meta auto t2) {
	return Value<std::same_as<decltype(t1), decltype(t2)>>;
}

namespace detail
{
	template<class F, list List>
	struct map;

	template<class F, template<class...> class L, type... Args>
	struct map<F, L<Args...>>
	{
		using type = L<decltype(F()(std::declval<Args>()))...>;
	};

	template<class F, template<class...> class L, value... Args>
	struct map<F, L<Args...>>
	{
		using type = L<detail::Value<F()(Args::value)>...>;
	};
}

constexpr auto map(auto F, type auto Type) {
	using a = detail::map<decltype(F), decltype(Type)::type>::type;
	return detail::Type<a>();
}

namespace detail
{
	template<class F, list L>
	struct filter;

	template<class F, template<class...> class L, class Arg>
	struct filter<F, L<Arg>>
	{
		using type = std::conditional_t<
			std::invoke_result_t<F, Arg>::value,
			L<Arg>,
			L<>
		>;
	};

	template<class F, template<class...> class L, class Arg, class... Args>
	struct filter<F, L<Arg, Args...>>
	{
		using next = detail::filter<F, L<Args...>>::type;

		using type = std::conditional_t<
			std::invoke_result_t<F, Arg>::value,
			typename detail::prepend<next, Arg>::type,
			next
		>;
	};
}

template<class F, list L>
using filter_t = detail::filter<F, L>::type;

template<class F, list L>
constexpr auto filter(F, Type_t<L>) {
	return Type<filter_t<F, L>>;
}

constexpr auto operator|(auto f1, auto f2) {
	return [=](auto x) {
		return f1(f2(x));
	};
}

int main() {
	//constexpr std::tuple<detail::Type<float>, Type<int>> t;

	constexpr auto l = type_list<float, int, float>;

	//constexpr auto l = type_list<Value<1>, Value<2>, Value<3>>;
	constexpr int r = 1;
	constexpr auto l2 = value_list<1, 2, 3>;

	constexpr auto a = map(
		[](auto a) {
			return Type<double>;
		}, l
	);

	constexpr auto l3 = map(
		[](auto i) {
			return i * 2;
		},
		l2
			);

	//constexpr auto l4 = filter(
	//	[](auto a) {
	//		return true;
	//	},
	//	l
	//		);

	using oh = decltype(a)::type;
	oh o;


	constexpr auto b = list<std::tuple<int>>;
	constexpr auto b2 = list<int>;

	//constexpr auto aaaaaaaaaaaaa = detail::list_cat<>;


	constexpr auto l5 = value_list<1, 2, 3, 4, 5, 6>;


	constexpr auto f = []<type T>(T t) {
		return std::same_as<T::type, float>;
	};


	constexpr auto g = []<auto v>(Value_t<v>) {
		return Value<v % 2 == 0>;
	};

	constexpr auto g1 = []<auto v>(Value_t<v>) {
		return Value<v + 1>;
	};

	constexpr auto gg = g1 | g1;
	constexpr auto ggg = gg | g1;
	constexpr auto gggg = g1 | ggg;

	constexpr auto zz = filter(g | ggg, l5);

	rand();
	return 0;
}