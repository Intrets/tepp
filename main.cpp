#include <concepts>
#include <tuple>

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
constexpr auto list_size = detail::list_size<L>::value;

namespace detail
{
	//template<list L>
	//struct to_tuple;

	//template<template<class... > class L, class... Args>
	//struct to_tuple<L<Args...>>
	//{
	//	using type = std::tuple<Args...>;
	//};

	template<list From, template<class...> class To>
	struct to_list;

	template<template<class...> class From, class... Args, template<class...> class To>
	struct to_list<From<Args...>, To>
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

template<list L>
constexpr auto to_tuple = detail::to_list<L, std::tuple>::type;

namespace detail
{
	template<list L>
	struct list_cat;

	template<template<list...> class L>
	struct list_cat<L<>>
	{
		using type = L<>;
	};

	template<template<list...> class L, list Arg, list... Args>
	struct list_cat<L<Arg, Args...>>
	{
		using next = detail::list_cat<L<Args...>>::type;

		using type = std::conditional_t <
			detail::list_size<Arg>::value == 0,
			next,
			typename detail::prepend<next, Arg>::type
		>;
	};
}

namespace detail
{
	template<list List>
	struct size;

	template<template<class...> class L, class...Args>
	struct size<L<Args...>>
	{
		static constexpr auto value = sizeof...(Args);
	};
}


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

template<class V>
concept value = detail::is_value<V>::value;

template<class T>
concept meta = value<T> || type<T>;

//template<value... Values>
//constexpr auto value_list = Type<std::tuple<Values>...>;
constexpr auto value_list(value auto... Values) {
	return Type<std::tuple<decltype(Values)...>>;
}

template<class... Args>
constexpr auto make_list = Type<std::tuple<detail::Type<Args>...>>;

consteval auto operator== (meta auto t1, meta auto t2) {
	return Value<std::same_as<decltype(t1), decltype(t2)>>;
}

namespace detail
{
	template<class F, list List>
	struct map2;

	template<class F, template<class...> class L, type... Args>
	struct map2<F, L<Args...>>
	{
		using type = L<decltype(F()(std::declval<Args>()))...>;
	};

	template<class F, template<class...> class L, value... Args>
	struct map2<F, L<Args...>>
	{
		using type = L<detail::Value<F()(Args::value)>...>;
	};
}

constexpr auto map(auto F, type auto Type) {
	using a = detail::map2<decltype(F), decltype(Type)::type>::type;
	return detail::Type<a>();
}

namespace detail
{
	template<auto F, list List>
	struct filter;

	template<auto F, template<class...> class L, type... Args>
	struct filter<F, L<Args...>>
	{
		//using mapped =
		//using type = decltype(std::tuple_cat(
		//	std::conditional_t<
		//	(F(std::declval<Args>())),
		//	std::tuple<Args>,
		//	std::tuple<>
		//	>()...
		//));
	};

	template<auto F, template<class...> class L, value... Args>
	struct filter<F, L<Args...>>
	{
		//using type = decltype(std::tuple_cat(
		//	std::conditional_t<
		//	(F(Args::value)),
		//	std::tuple<Args>,
		//	std::tuple<>
		//	>()...
		//));
	};
}

//constexpr auto filter(auto F, type auto Type) {
//	using a = detail::filter<F, decltype(Type)::type>::type;
//	return detail::Type<a>();
//}

//constexpr auto filter(auto F, type auto Type) {
//	using r = detail::filter<F, decltype(Type)>;
//	return ::Type<r>;
//}

//template<class F, type... T>
//constexpr auto filter(F f, T... type) {
//	using a = std::tuple_cat<
//		std::conditional_t<
//		f(type),
//		std::tuple<decltype(type)>,
//		std::tuple<>
//		>...>;
//
//	return Type<a>;
//}

consteval auto testing() {
	constexpr auto f1 = Type<float>;
	constexpr auto f2 = Type<float>;
	constexpr auto i1 = Type<int>;

	constexpr auto v1 = Value<1>;
	constexpr auto v2 = Value<1>;
	constexpr auto v3 = Value<2>;

	//constexpr auto b1 = f1 == f2;
	//constexpr auto b2 = f1 == i1;

	constexpr auto b3 = v1 == v1;
	constexpr auto b4 = v1 == v2;
	constexpr auto b5 = v1 == v3;

}


int main() {
	testing();

	//constexpr std::tuple<detail::Type<float>, Type<int>> t;

	constexpr auto l = make_list<float, int, float>;

	//constexpr auto l = make_list<Value<1>, Value<2>, Value<3>>;
	constexpr int r = 1;
	constexpr auto l2 = value_list(Value<1>, Value<2>, Value<3>);

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

	//using tt = detail::list_cat<std::tuple<std::tuple<int>, std::tuple<>, std::tuple<float>>>::type;
	//using tt = detail::list_cat<std::tuple<std::tuple<int>, std::tuple<>, std::tuple<float>>>::type;
	using tt = detail::list_cat<std::tuple<>>::type;

	tt t;

	rand();
	return 0;
}