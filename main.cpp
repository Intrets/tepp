#include <tuple>
#include <type_traits>
#include <concepts>

namespace detail
{
	template<class T>
	struct arg {};

	template<template<class> class F>
	struct wf
	{
		template<class T>
		using apply = F<T>;

		template<class T>
		using apply_t = typename apply<T>::type;

		template<class T>
		static constexpr decltype(apply<T>::type::value) value = apply<T>::type::value;
	};
}

template<class T>
constexpr auto arg = detail::arg<T>();

template<class R>
struct is_same
{
	template<class T>
	using type = std::is_same<R, T>;
};

struct inspect_type
{
	template<class T>
	using type = typename T::type;
};


template<class T, T t>
struct constant
{
	using type = T;
	static constexpr T value = t;
};

struct inspect_value
{
	template<class T>
	using type = constant<std::remove_cvref_t<decltype(T::value)>, T::value>;
};

template<auto I>
struct greater_than
{
	template<class T>
	using type = std::conditional_t<
		(T::value > I),
		std::true_type,
		std::false_type
	>;
};

struct even_type
{
	template<class T>
	using type = std::conditional_t<
		(T::value % 2 == 0),
		std::true_type,
		std::false_type
	>;
};


constexpr auto neg = detail::wf<std::negation>();
constexpr auto type = detail::wf<inspect_type::template type>();
constexpr auto value = detail::wf<inspect_value::template type>();
constexpr auto even = detail::wf<even_type::template type>();

template<class T>
constexpr auto is = detail::wf<is_same<T>::template type>();

template<auto I>
constexpr auto gt = detail::wf<greater_than<I>::template type>();


template<template <class> class F1, template <class> class F2>
struct compose
{
	template<class T>
	using type = F1<F2<T>>;
};

template<template <class> class F1, template <class> class F2>
struct applicative_and
{
	template<class T>
	using type = std::conjunction<F1<T>, F2<T>>;
};


template<template<class> class P>
constexpr auto wf = detail::wf<P>();

#define to_constraint(X) evaluate<decltype(X)>

template<class T, class WF>
concept evaluate = WF::template value<T>;


namespace detail
{
	template<class WF, class Result, class Tuple>
	struct filter;

	template<class WF, class Result >
	struct filter<WF, Result, std::tuple<>>
	{
		using type = Result;
	};

	template<class WF, class... RArgs, class Arg, class... Args>
	struct filter<WF, std::tuple<RArgs...>, std::tuple<Arg, Args...>>
	{
		using type = std::conditional_t<
			WF::template value<Arg>,
			typename filter<WF, std::tuple<RArgs..., Arg>, std::tuple<Args...>>::type,
			typename filter<WF, std::tuple<RArgs...>, std::tuple<Args...>>::type
		>;
	};

	template<class WF, class Tuple>
	struct map;

	template<class WF, class... Args>
	struct map<WF, std::tuple<Args...>>
	{
		using type = std::tuple<typename WF::template apply<Args>...>;
	};
}

template<auto WF, class Tuple>
using filter_t = typename detail::filter<decltype(WF), std::tuple<>, Tuple>::type;

template<class WF>
struct filter_type
{
	template<class Tuple>
	using type = typename detail::filter<WF, std::tuple<>, Tuple>::type;
};

template<class WF>
struct map_type
{
	template<class Tuple>
	using type = typename detail::map<WF, Tuple>::type;
};

template<auto WF, class T>
using apply = typename decltype(WF)::template apply<T>;

template<template <class> class F1, template <class> class F2>
constexpr auto operator&&(detail::wf<F1>, detail::wf<F2>) {
	return detail::wf<applicative_and<F1, F2>::template type>();
}

template<template <class> class F1, template <class> class F2>
constexpr auto operator|(detail::wf<F1>, detail::wf<F2>) {
	return detail::wf<compose<F1, F2>::template type>();
}

template<template <class> class F1, class T>
constexpr auto operator|(detail::wf<F1>, detail::arg<T>) {
	return detail::wf<F1>::template value<T>;
}

template<template <class> class F1, class T>
requires requires () { detail::wf<F1>::template apply<T>(); }
constexpr auto operator|(detail::wf<F1>, detail::arg<T>) {
	return detail::wf<F1>::template apply<T>();
}

template<class WF>
constexpr auto filter(WF) {
	return detail::wf<filter_type<WF>::template type>();
}

template<class WF>
constexpr auto map(WF) {
	return detail::wf<map_type<WF>::template type>();
}


template<int N>
struct int_type
{
	using type = int;
	static constexpr int value = N;
};

template<int N>
struct float_type
{
	using type = float;
	static constexpr int value = N;
};

template<class T>
struct id
{
	using type = T;
	static constexpr auto value = T::value;
};


//struct sum2
//{
//	template<class T /*constant<1>*/>
//	using type = constant<typename T::type, T::value>;
//};
//
//struct sum1
//{
//	template<class T>
//	using type = detail::wf<add_type<detail::wf<sum2::template type>, T>::template type>;
//};
//
//struct sum0
//{
//	template<class T>
//	using type = detail::wf<add_type<detail::wf<sum1::template type>, T>::template type>;
//};

// wf::apply<1> -> wf::apply<2> -> 1 + 2


template<class WF, class I>
struct add_type
{
	template<class T>
	using type = constant<typename I::type, I::value + WF::template apply<T>::value>;
};

template<class WF, class I>
using add_wf = detail::wf<add_type<WF, I>::template type>;

struct sum0_struct
{
	template<class T>
	using type = add_wf<detail::wf<id>, T>;
};

struct sum1_struct
{
	template<class T>
	using type = add_wf<detail::wf<sum0_struct::template type>, T>;
};

using sum0_wf = detail::wf<sum0_struct::template type>;
using sum1_wf = detail::wf<sum1_struct::template type>;


//constexpr auto sum1 = sum1_struct();




int main() {

	using zz = add_wf<detail::wf<id>, constant<int, 1>>;
	using zz2 = detail::wf<add_type<detail::wf<id>, constant<int, 1>>::template type>;

	using zzz = add_wf<zz, constant<int, 1>>;
	//using zzz2 = detail::wf<add_type<zz2, constant<int, 1>>::template type>;
	//using zzz3 = detail::wf<add_type<zz2, constant<int, 1>>::template type>::apply<constant<int, 2>>;
	//using zzz4 = add_type<zz2, constant<int, 1>>::template type<constant<int, 2>>;
	//using zzz5 = constant<constant<int, 2>::type, constant<int, 1>::value + zz2::template value<constant<int, 2>>>;
	//using zzz6 = constant<int, 1 + zz2::template value<constant<int, 2>>>;
	//using zzz7 = constant<int, 1 + detail::wf<add_type<detail::wf<id>, constant<int, 1>>::template type>::template value<constant<int, 2>>>;



	//zzz::apply<constant<int, 3>>;
	using aa = zz::apply<constant<int, 2>>;
	using aa2 = zzz::apply<constant<int, 2>>;
	//zzz zzz1;

	//using ok = sum1_wf::template apply<constant<int, 1>>::template apply<constant<int, 2>>;
	using ok2 = sum0_wf::template apply<constant<int, 1>>;
	using ok3 = ok2::template apply<constant<int, 2>>;
	ok2 okok;
	ok3 okokok;



	//constant<int, 1>::value;


	//auto constexpr ok = sum1::type<constant<int, 1>>::value<constant<int, 2>>;
	//detail::wf<sum2::template type>::apply<constant<int, 1>> ak;

	//add_type<detail::wf<sum2::template type>, constant<int, 2>>::template type<constant<int, 1>> ik;
	//sum0::template type<constant<int, 2>>::template apply<constant<int, 1>> uuj;

	//add_type<detail::wf<sum1::template type>, constant<int, 2>>::template type<constant<int, 1>> ik1;

	rand();




	using tuple = std::tuple<
		int_type<0>,
		float_type<1>,
		float_type<2>,
		int_type<3>,
		float_type<4>,
		int_type<5>
	>;

	//using filtered_tuple = filter_t<neg | is<int> | type, tuple>;
	//using filtered_tuple = decltype(filter(neg | is<int> | type) | arg<tuple>);
	using filtered_tuple = apply<filter(neg | is<int> | type), tuple>;

	static_assert(std::is_same_v<
		filtered_tuple,
		std::tuple<float_type<1>, float_type<2>, float_type<4>>
	>);


	//using filtered_tuple_2 = filter_t<gt<1> && (neg | is<int> | type), tuple>;
	//using filtered_tuple_2 = decltype(filter(gt<1> && (neg | is<int> | type)) | arg<tuple>);
	using filtered_tuple_2 = apply<filter(gt<1> && (neg | is<int> | type)), tuple>;

	static_assert(std::is_same_v<
		filtered_tuple_2,
		std::tuple<float_type<2>, float_type<4>>
	>);


	using filtered_tuple_3 = filter_t<even | value, tuple>;

	static_assert(std::is_same_v<
		filtered_tuple_3,
		std::tuple<int_type<0>, float_type<2>, float_type<4>>
	>);


	using nested_tuple = std::tuple<std::tuple<int_type<0>, float_type<1>>, std::tuple<int_type<2>, float_type<3>>>;

	using z = apply<map(filter(is<int> | type)), nested_tuple>;

	static_assert(std::is_same_v<
		z,
		std::tuple<std::tuple<int_type<0>>, std::tuple<int_type<2>>>
	>);

	return 0;
}