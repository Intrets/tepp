// tepp - a template library for c++
// Copyright (C) 2022 intrets

#pragma once

#include <functional>
#include <span>
#include <tuple>
#include <type_traits>

#include "tepp/integers.h"

namespace te
{
	namespace detail
	{
		template<class T>
		concept has_value =
		    requires(T t) {
			    T::value;
			    t.value;
		    };

		template<class T>
		struct Type;

		template<has_value T>
		struct Type<T>
		{
			using type = T;
			constexpr static auto value = T::value;
		};

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

#define Decltype(X) te::Type<decltype(X)>
#define Gettype(X) typename decltype(X)::type

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

			constexpr operator value_type() {
				return value;
			}
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
	using Value_t = detail::Type<detail::Value<v>>;

	template<auto v>
	constexpr auto Value = Value_t<v>();

	template<class V>
	concept value = detail::is_value<V>::value;

#define Getvalue(X) decltype(X)::type::value

	namespace detail
	{
		template<template<class...> class F, bool returns_value>
		struct type_function;

		template<template<class...> class F>
		struct type_function<F, false>
		{
			template<class... T>
			constexpr auto operator()(Type_t<T>...) const {
				return detail::Type<F<T...>>();
			}
		};

		template<template<class...> class F>
		struct type_function<F, true>
		{
			template<class... T>
			constexpr auto operator()(Type_t<T>...) const {
				return detail::Type<detail::Value<F<T...>::value>>();
			}
		};

		template<class T>
		struct is_type_function;

		template<template<class...> class T, bool returns_value>
		struct is_type_function<type_function<T, returns_value>> : std::true_type
		{
		};

		template<class T>
		struct is_type_function : std::false_type
		{
		};
	}

	template<class T>
	concept type_function_c = detail::is_type_function<T>::value;

	template<template<class...> class F>
	using type_function_t = detail::type_function<F, false>;

	template<template<class...> class F>
	constexpr auto Type_function = type_function_t<F>();

	template<template<class...> class F>
	using value_function_t = detail::type_function<F, true>;

	template<template<class...> class F>
	constexpr auto Value_function = value_function_t<F>();

	template<class... Args>
	struct list_type
	{
	};

	template<integer_t I, class L>
	struct list_element;

	template<template<class...> class L, class Arg0, class... Args>
	struct list_element<0, L<Arg0, Args...>>
	{
		using type = Arg0;
	};

	template<template<class...> class L, class Arg0, class Arg1, class... Args>
	struct list_element<1, L<Arg0, Arg1, Args...>>
	{
		using type = Arg1;
	};

	template<template<class...> class L, class Arg0, class Arg1, class Arg2, class... Args>
	struct list_element<2, L<Arg0, Arg1, Arg2, Args...>>
	{
		using type = Arg2;
	};

	template<template<class...> class L, class Arg0, class Arg1, class Arg2, class Arg3, class... Args>
	struct list_element<3, L<Arg0, Arg1, Arg2, Arg3, Args...>>
	{
		using type = Arg3;
	};

	template<template<class...> class L, class Arg0, class Arg1, class Arg2, class Arg3, class Arg4, class... Args>
	struct list_element<4, L<Arg0, Arg1, Arg2, Arg3, Arg4, Args...>>
	{
		using type = Arg4;
	};

	template<template<class...> class L, class Arg0, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class... Args>
	struct list_element<5, L<Arg0, Arg1, Arg2, Arg3, Arg4, Arg5, Args...>>
	{
		using type = Arg5;
	};

	template<template<class...> class L, class Arg0, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class... Args>
	struct list_element<6, L<Arg0, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Args...>>
	{
		using type = Arg6;
	};

	template<template<class...> class L, class Arg0, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class... Args>
	struct list_element<7, L<Arg0, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Args...>>
	{
		using type = Arg7;
	};

	template<template<class...> class L, class Arg0, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class... Args>
	struct list_element<8, L<Arg0, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Args...>>
	{
		using type = Arg8;
	};

	template<template<class...> class L, class Arg0, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9, class... Args>
	struct list_element<9, L<Arg0, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9, Args...>>
	{
		using type = Arg9;
	};

	template<template<class...> class L, class Arg0, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9, class Arg10, class... Args>
	struct list_element<10, L<Arg0, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9, Arg10, Args...>>
	{
		using type = Arg10;
	};

	template<template<class...> class L, class Arg0, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9, class Arg10, class Arg11, class... Args>
	struct list_element<11, L<Arg0, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9, Arg10, Arg11, Args...>>
	{
		using type = Arg11;
	};

	template<template<class...> class L, class Arg0, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9, class Arg10, class Arg11, class Arg12, class... Args>
	struct list_element<12, L<Arg0, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9, Arg10, Arg11, Arg12, Args...>>
	{
		using type = Arg12;
	};

	template<template<class...> class L, class Arg0, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9, class Arg10, class Arg11, class Arg12, class Arg13, class... Args>
	struct list_element<13, L<Arg0, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9, Arg10, Arg11, Arg12, Arg13, Args...>>
	{
		using type = Arg13;
	};

	template<template<class...> class L, class Arg0, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9, class Arg10, class Arg11, class Arg12, class Arg13, class Arg14, class... Args>
	struct list_element<14, L<Arg0, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9, Arg10, Arg11, Arg12, Arg13, Arg14, Args...>>
	{
		using type = Arg14;
	};

	template<template<class...> class L, class Arg0, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9, class Arg10, class Arg11, class Arg12, class Arg13, class Arg14, class Arg15, class... Args>
	struct list_element<15, L<Arg0, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9, Arg10, Arg11, Arg12, Arg13, Arg14, Arg15, Args...>>
	{
		using type = Arg15;
	};

	template<template<class...> class L, class Arg0, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9, class Arg10, class Arg11, class Arg12, class Arg13, class Arg14, class Arg15, class Arg16, class... Args>
	struct list_element<16, L<Arg0, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9, Arg10, Arg11, Arg12, Arg13, Arg14, Arg15, Arg16, Args...>>
	{
		using type = Arg16;
	};

	template<integer_t I, template<class...> class L, class Arg0, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9, class Arg10, class Arg11, class Arg12, class Arg13, class Arg14, class Arg15, class Arg16, class... Args>
	struct list_element<I, L<Arg0, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9, Arg10, Arg11, Arg12, Arg13, Arg14, Arg15, Arg16, Args...>>
	{
		using type = list_element<I - 16, L<Args...>>::type;
	};

	template<integer_t I, class L>
	using list_element_t = list_element<I, L>::type;

	template<class... Args>
	constexpr auto List = Type<te::list_type<Args...>>;

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

	template<list L>
	constexpr auto List_size = Value<list_size_v<L>>;

	constexpr auto list_size = []<list L>(Type_t<L>) {
		return Value<list_size_v<L>>;
	};

	template<list L>
	constexpr bool is_empty_v = detail::list_size<L>::value == 0;

	namespace detail
	{
		template<list From, type_function_c To>
		struct convert_list;

		template<template<class...> class From, class... Args, type_function_c To>
		struct convert_list<From<Args...>, To>
		{
			using type = typename std::invoke_result_t<To, Type_t<Args>...>::type;
		};

		template<list L, class... Args>
		struct same_type_list;

		template<template<class...> class L, class... Olds, class... Args>
		struct same_type_list<L<Olds...>, Args...>
		{
			using type = L<Args...>;
		};

		template<template<class...> class T2, list L>
		struct is_same_list_type;

		template<template<class...> class T2, template<class...> class T1, class... Args>
		struct is_same_list_type<T2, T1<Args...>>
		{
			constexpr static bool value = std::same_as<T1<Args...>, T2<Args...>>;
		};

		template<class T, list L>
		struct append;

		template<class T, template<class...> class L, class... Args>
		struct append<T, L<Args...>>
		{
			using type = L<Args..., T>;
		};

		template<class T, list L>
		struct prepend;

		template<class T, template<class...> class L, class... Args>
		struct prepend<T, L<Args...>>
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

		template<list L>
		struct tail;

		template<template<class...> class L, class Arg, class... Args>
		struct tail<L<Arg, Args...>>
		{
			using type = L<Args...>;
		};
	}

	template<type_function_c To, list L>
	using convert_to_t = typename detail::convert_list<L, To>::type;

	template<template<class...> class To, list From>
	struct convert_list_type;

	template<template<class...> class To, template<class...> class From, class... Args>
	struct convert_list_type<To, From<Args...>>
	{
		using type = To<Args...>;
	};

	template<template<class...> class To, list From>
	using convert_list_type_t = convert_list_type<To, From>::type;

	constexpr auto convert_to = []<type_function_c To, list List>(Type_t<To>, Type_t<List>) {
		return Type<convert_to_t<To, List>>;
	};

	template<list L, class... Args>
	using same_type_list_t = typename detail::same_type_list<L, Args...>::type;

	template<template<class...> class T2, list L>
	constexpr bool is_same_list_type_v = detail::is_same_list_type<T2, L>::value;

	template<class L, template<class...> class T2>
	concept same_list_type = is_same_list_type_v<T2, L>;

	template<list L>
	using head_t = typename detail::head<L>::type;

	constexpr auto head = []<list List>(Type_t<List>) {
		return Type<head_t<List>>;
	};

	template<list L>
	using tail_t = typename detail::tail<L>::type;

	constexpr auto tail = []<list List>(Type_t<List>) {
		return Type<tail_t<List>>;
	};

	template<class T, list L>
	using prepend_t = typename detail::prepend<T, L>::type;

	template<class T, list L>
	using append_t = typename detail::append<T, L>::type;

	namespace detail
	{
		template<list L>
		struct list_cat;

		template<template<class...> class L, list Arg>
		struct list_cat<L<Arg>>
		{
			using type = std::conditional_t<
			    list_size_v<Arg> == 0,
			    L<>,
			    L<Arg>>;
		};

		template<template<class...> class L, list Arg, list... Args>
		struct list_cat<L<Arg, Args...>>
		{
			using next = typename detail::list_cat<L<Args...>>::type;

			using type = std::conditional_t<
			    list_size_v<Arg> == 0,
			    next,
			    typename detail::prepend<Arg, next>::type>;
		};
	}

	template<list List>
	using list_cat_t = typename detail::list_cat<List>::type;

	template<class T>
	concept meta = value<T> || type<T>;

	template<auto... values>
	using value_list_t = detail::Type<list_type<detail::Value<values>...>>;

	template<auto... values>
	constexpr auto value_list = value_list_t<values...>();

	template<class... Args>
	using type_list_t = te::list_type<Args...>;

	template<class... Args>
	constexpr auto Type_list = Type<type_list_t<Args...>>;

	constexpr auto type_list = []<class... Args>(Type_t<Args>...) {
		return Type_list<Args...>;
	};

	constexpr auto same = []<class T1, class T2>(Type_t<T1>, Type_t<T2>) {
		return Value<std::same_as<T1, T2>>;
	};

	template<meta T1, meta T2>
	consteval auto operator==(T1, T2) {
		return Value<std::same_as<T1, T2>>;
	}

	namespace detail
	{
		template<class F, list List>
		struct map;

		template<class F, template<class...> class L, class... Args>
		struct map<F, L<Args...>>
		{
		private:
			template<class F1, class Arg1>
			using applied = typename std::invoke_result_t<F1, Type_t<Arg1>>::type;

		public:
			using type = L<applied<F, Args>...>;
		};
	}

	template<class F, list L>
	using map_t = typename detail::map<F, L>::type;

	template<class F, list L>
	constexpr auto map(F, Type_t<L>) {
		return Type<map_t<F, L>>;
	}

	namespace detail
	{
		template<class F, list L>
		struct filter;

		template<class F, template<class...> class L, class Arg>
		struct filter<F, L<Arg>>
		{
			using type = std::conditional_t<
			    std::invoke_result_t<F, Type_t<Arg>>::value,
			    L<Arg>,
			    L<>>;
		};

		template<class F, template<class...> class L, class Arg, class... Args>
		struct filter<F, L<Arg, Args...>>
		{
			using next = typename detail::filter<F, L<Args...>>::type;

			using type = std::conditional_t<
			    std::invoke_result_t<F, Type_t<Arg>>::value,
			    prepend_t<Arg, next>,
			    next>;
		};
	}

	template<class F, list L>
	using filter_t = typename detail::filter<F, L>::type;

	template<class F, list L>
	constexpr auto filter(F, Type_t<L>) {
		return Type<typename detail::filter<F, L>::type>;
	}

	constexpr auto apply = []<type_function_c F, template<class...> class L, class... Ls>(F f, Type_t<L<Ls...>>) {
		return f(Type<Ls>...);
	};

	namespace detail
	{
		template<class F, list L>
		struct all;

		template<class F, template<class...> class L, class... Args>
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

	constexpr auto is = []<class T>(T) {
		return []<class S>(S) {
			return Value<std::same_as<S, T>>;
		};
	};

	template<class T>
	using is_t = decltype(is(Type<T>));

	constexpr auto and_f = []<list L>(Type_t<L>) {
		return all(id, Type<L>);
	};

	namespace detail
	{
		constexpr auto brute_apply_test(auto f, auto arg) {
			return f(arg);
		}
	}

	template<auto v1, auto v2>
	constexpr auto operator||(Value_t<v1>, Value_t<v2>) {
		return Value < v1 || v2 > ;
	}

	template<auto v1, auto v2>
	constexpr auto operator&&(Value_t<v1>, Value_t<v2>) {
		return Value < v1 && v2 > ;
	}

	template<auto v>
	constexpr auto operator!(Value_t<v>) {
		return Value<!v>;
	}

	namespace detail
	{
		template<size_t N>
		struct string
		{
			constexpr string(char const (&str)[N]) {
				std::copy_n(str, N, value);
			}

			char value[N];
			static constexpr auto size = N;
		};
	}

	namespace detail
	{
		template<int I, list L, list R>
		struct reverse;

		template<list L, list R>
		struct reverse<0, L, R>
		{
			using value = R;
		};

		template<int I, list L, list R>
		struct reverse
		{
			using value = typename reverse<
			    I - 1,
			    tail_t<L>,
			    prepend_t<head_t<L>, R>>::value;
		};
	}

	template<list L>
	struct reverse
	{
		using value = typename detail::reverse<list_size_v<L>, L, same_type_list_t<L>>::value;
	};

	template<list L>
	using reverse_t = typename reverse<L>::value;

	template<class T, int I>
	struct enumeration
	{
		using type = T;
		static constexpr size_t index = I;
	};

	template<class List, class Is>
	struct enumerate;

	template<>
	struct enumerate<list_type<>, std::index_sequence<>>
	{
		using type = list_type<>;
	};

	template<class... Args, int... Is>
	struct enumerate<list_type<Args...>, std::index_sequence<Is...>>
	{
		using type = list_type<enumeration<Args, Is>...>;
	};

	namespace detail
	{
		constexpr static auto convert_index_sequence = []<auto... Is>(Type_t<std::index_sequence<Is...>>) {
			return value_list<Is...>;
		};
	}

	constexpr static auto index_sequence = []<auto I>(Value_t<I>) {
		return detail::convert_index_sequence(Type<std::make_index_sequence<I>>);
	};

	template<class List>
	using enumerate_t = typename enumerate<List, std::make_index_sequence<List::size>>::type;

	template<class L, class T>
	static constexpr bool contains_v = false;

	template<template<class...> class L, class... Args, class T>
	static constexpr bool contains_v<L<Args...>, T> = (std::same_as<Args, T> || ...);

	template<class T, class L>
	concept member_of = contains_v<L, T>;

	template<class T, class... Args>
	concept member_of2 = contains_v<te::list_type<Args...>, T>;

	template<class T>
	struct is_std_fun;

	template<class R, class... Args>
	struct is_std_fun<std::function<R(Args...)>> : std::true_type
	{
		using return_type = R;
		using arguments_list = list_type<Args...>;
	};

	template<class T>
	struct is_std_fun : std::false_type
	{
	};

	template<class T>
	inline constexpr bool is_std_fun_v = is_std_fun<T>::value;

	template<class T>
	concept std_fun = is_std_fun_v<T>;

	template<class T>
	struct is_c_fun;

	template<class R, class... Args>
	struct is_c_fun<R(Args...)> : std::true_type
	{
		using return_type = R;
		using arguments_list = list_type<Args...>;
	};

	template<class R, class... Args>
	struct is_c_fun<R (*)(Args...)> : std::true_type
	{
		using return_type = R;
		using arguments_list = list_type<Args...>;
	};

	template<class T>
	struct is_c_fun : std::false_type
	{
	};

	template<class T>
	inline constexpr bool is_c_fun_v = is_c_fun<T>::value;

	template<class T>
	concept c_fun = is_c_fun_v<T>;

	template<class T>
	struct is_member_fun;

	template<class R, class B, class... Args>
	struct is_member_fun<R (B::*)(Args...)> : std::true_type
	{
		using return_type = R;
		using arguments_list = list_type<Args...>;
		using base_class = B;
	};

	template<class T>
	struct is_member_fun : std::false_type
	{
	};

	template<class T>
	inline constexpr bool is_member_fun_v = is_member_fun<T>::value;

	template<class T>
	concept member_fun = is_member_fun_v<T>;

	template<class, class = void>
	struct is_lambda_fun : std::false_type
	{
	};

	template<class T>
	struct is_lambda_fun<T, std::void_t<decltype(&T::operator())>>
	{
		static constexpr bool value = !is_std_fun<T>::value;
	};

	template<class T>
	inline constexpr bool is_lambda_fun_v = is_lambda_fun<T>::value;

	template<class T>
	concept lambda_fun = is_lambda_fun_v<T>;

	template<class T>
	struct deconstruct_fun;

	template<c_fun T>
	struct deconstruct_fun<T>
	{
		using return_type = typename is_c_fun<T>::return_type;
		using arguments_list = typename is_c_fun<T>::arguments_list;
	};

	template<std_fun T>
	struct deconstruct_fun<T>
	{
		using return_type = typename is_std_fun<T>::return_type;
		using arguments_list = typename is_std_fun<T>::arguments_list;
	};

	template<class T>
	struct deconstruct_lambda_member_fun;

	template<class R, class B, class... Args>
	struct deconstruct_lambda_member_fun<R (B::*)(Args...) const>
	{
		using return_type = R;
		using arguments_list = list_type<Args...>;
	};

	template<lambda_fun T>
	struct deconstruct_fun<T>
	{
		using return_type = typename deconstruct_lambda_member_fun<decltype(&T::operator())>::return_type;
		using arguments_list = typename deconstruct_lambda_member_fun<decltype(&T::operator())>::arguments_list;
	};

	template<member_fun T>
	struct deconstruct_fun<T>
	{
		using return_type = typename is_member_fun<T>::return_type;
		using arguments_list = typename is_member_fun<T>::arguments_list;
		using base_class = typename is_member_fun<T>::base_class;
	};

	template<class T>
	using return_type_t = typename deconstruct_fun<T>::return_type;

	template<auto ptr>
	using return_type_t_ptr = typename deconstruct_fun<decltype(ptr)>::return_type;

	template<class T>
	using arguments_list_t = typename deconstruct_fun<T>::arguments_list;

	template<class T>
	constexpr auto Arguments_list = Type<arguments_list_t<T>>;

	template<class T>
	using member_function_base_class_t = typename deconstruct_fun<T>::base_class;

	template<class T>
	inline constexpr bool is_any_fun_v = is_c_fun_v<T> || is_lambda_fun_v<T> || is_member_fun_v<T> || is_std_fun_v<T>;

	namespace impl
	{
		struct nothing
		{
		};

		template<class T, class... Args>
		concept number_arguments_concept =
		    requires(T t) {
			    t(std::declval<Args>()...);
		    };

		template<class T, list List>
		struct number_arguments_test;

		template<template<class...> class L, class T, class... Args>
		struct number_arguments_test<T, L<Args...>>
		{
			constexpr static bool value = number_arguments_concept<T, Args...>;
		};
	}

	template<int I, class T>
	struct replicate;

	template<int I, class T>
	using replicate_t = typename replicate<I, T>::type;

	template<class T>
	struct replicate<0, T>
	{
		using type = list_type<>;
	};

	template<class T>
	struct replicate<1, T>
	{
		using type = list_type<T>;
	};

	template<int I, class T>
	struct replicate
	{
		using next = typename replicate<I - 1, T>::type;
		using type = prepend_t<T, next>;
	};

	namespace detail
	{
		template<list L1, list L2>
		struct zip;

		template<template<class...> class L1, template<class...> class L2, class... Args1, class... Args2>
		struct zip<L1<Args1...>, L2<Args2...>>
		{
			using type = te::list_type<te::list_type<Args1, Args2>...>;
		};

		template<list L1, list L2, list L3>
		struct zip3;

		template<template<class...> class L1, template<class...> class L2, class... Args1, class... Args2, template<class...> class L3, class... Args3>
		struct zip3<L1<Args1...>, L2<Args2...>, L3<Args3...>>
		{
			using type = te::list_type<te::list_type<Args1, Args2, Args3>...>;
		};

		template<list L1, list L2, list L3, list L4>
		struct zip4;

		template<template<class...> class L1, template<class...> class L2, class... Args1, class... Args2, template<class...> class L3, class... Args3, template<class...> class L4, class... Args4>
		struct zip4<L1<Args1...>, L2<Args2...>, L3<Args3...>, L4<Args4...>>
		{
			using type = te::list_type<te::list_type<Args1, Args2, Args3, Args4>...>;
		};

		template<list L1, list L2, list L3, list L4, list L5>
		struct zip5;

		template<template<class...> class L1, template<class...> class L2, class... Args1, class... Args2, template<class...> class L3, class... Args3, template<class...> class L4, class... Args4, template<class...> class L5, class... Args5>
		struct zip5<L1<Args1...>, L2<Args2...>, L3<Args3...>, L4<Args4...>, L5<Args5...>>
		{
			using type = te::list_type<te::list_type<Args1, Args2, Args3, Args4, Args5>...>;
		};
	}

	constexpr static auto zip = []<list L1, list L2>(Type_t<L1>, Type_t<L2>) {
		return Type<typename detail::zip<L1, L2>::type>;
	};

	constexpr static auto zip3 = []<list L1, list L2, list L3>(Type_t<L1>, Type_t<L2>, Type_t<L3>) {
		return Type<typename detail::zip3<L1, L2, L3>::type>;
	};

	constexpr static auto zip4 = []<list L1, list L2, list L3, list L4>(Type_t<L1>, Type_t<L2>, Type_t<L3>, Type_t<L4>) {
		return Type<typename detail::zip4<L1, L2, L3, L4>::type>;
	};

	constexpr static auto zip5 = []<list L1, list L2, list L3, list L4, list L5>(Type_t<L1>, Type_t<L2>, Type_t<L3>, Type_t<L4>, Type_t<L5>) {
		return Type<typename detail::zip5<L1, L2, L3, L4, L5>::type>;
	};

	constexpr static auto enumerate_in_list = []<list L>(Type_t<L>) {
		return zip(Type<L>, index_sequence(List_size<L>));
	};

	template<class T, int I>
	concept number_of_arguments_is =
	    impl::number_arguments_test<T, replicate_t<I, impl::nothing>>::value ||
	    ((requires(T t) { &T::operator(); }) && te::arguments_list_t<decltype(&T::operator())>::size == I) ||
	    ((te::is_c_fun_v<T> || te::is_lambda_fun_v<T>) && te::arguments_list_t<T>::size == I);

	namespace detail
	{
		template<class Is, class... Ts>
		struct tuple_zip;

		template<class... Ts, size_t... I>
		struct tuple_zip<std::index_sequence<I...>, Ts...>
		{
			template<size_t i>
			constexpr static auto apply2(Ts&&... ts) {
				return std::tuple<std::tuple_element_t<i, std::remove_cvref_t<Ts>>...>(std::get<i>(ts)...);
			}

			constexpr static auto apply(Ts&&... ts) {
				return std::make_tuple(
				    apply2<I>(std::forward<Ts>(ts)...)...
				);
			}
		};
	}

	template<class T1, class T2>
	concept same_tuple_size = (std::tuple_size_v<std::remove_cvref_t<T1>> == std::tuple_size_v<std::remove_cvref_t<T2>>);

	template<class T, same_tuple_size<T>... Ts>
	constexpr static auto tuple_zip(T&& t, Ts&&... ts) {
		constexpr size_t size = std::tuple_size_v<std::remove_cvref_t<T>>;

		return detail::tuple_zip<
		    std::make_index_sequence<size>,
		    T,
		    Ts...>::apply(std::forward<T>(t), std::forward<Ts>(ts)...);
	}

	namespace detail
	{
		template<class F, class T, class IS>
		struct for_each;

		template<class F, class T, integer_t... I>
		struct for_each<F, T, std::integer_sequence<integer_t, I...>>
		{
			constexpr static auto apply(F&& f, T&& t) {
				(f(std::get<I>(t)), ...);
			}
		};

		template<integer_t I0, class IS, class F>
		struct for_iota;

		template<class F, integer_t I0, integer_t... I>
		struct for_iota<I0, std::integer_sequence<integer_t, I...>, F>
		{
			constexpr static auto apply(F&& f) {
				(f(te::Value<I0 + I>), ...);
			}
		};
	}

	template<class F, class Tuple>
	constexpr static auto tuple_for_each(F&& f, Tuple&& t) {
		constexpr integer_t size = static_cast<integer_t>(std::tuple_size_v<std::remove_cvref_t<Tuple>>);

		detail::for_each<F, Tuple, std::make_integer_sequence<integer_t, size>>::apply(std::forward<F>(f), std::forward<Tuple>(t));
	}

	template<integer_t I0, integer_t I, class F>
	constexpr static auto for_iota(F&& f) {
		detail::for_iota<I0, std::make_integer_sequence<integer_t, I - I0>, F>::apply(std::forward<F>(f));
	}

	template<integer_t I, class F>
	constexpr static auto for_iota(F&& f) {
		for_iota<0, I>(std::forward<F>(f));
	}

	constexpr static auto for_each(auto&& f, auto&&... args) {
		(f(std::forward<decltype(args)>(args)), ...);
	}

	namespace detail
	{
		template<class Is>
		struct repeat_t;

		template<integer_t... I>
		struct repeat_t<std::integer_sequence<integer_t, I...>>
		{
			template<integer_t, class F>
			constexpr static void applyHelper(F& f) {
				std::invoke(f);
			}

			template<class F>
			constexpr static void apply(F&& f) {
				(applyHelper<I>(f), ...);
			}
		};

		template<integer_t I>
		using repeat = repeat_t<std::make_integer_sequence<integer_t, I>>;
	}

	template<size_t I, class F>
	constexpr static void repeat(F&& f) {
		if constexpr (I != 0) {
			detail::repeat<I>::apply(std::forward<F>(f));
		}
	}

	namespace detail
	{
		template<class F, list L>
		struct for_each_type;

		template<class F, template<class...> class L, class... Args>
		struct for_each_type<F, L<Args...>>
		{
			constexpr static void apply(F&& f) {
				(f(Type<Args>()), ...);
			}
		};
	}

	constexpr static auto for_each_type = []<list L, class F>(F&& f, Type_t<L>) {
		detail::for_each_type<F, L>::apply(std::forward<F>(f));
	};

	constexpr static auto tie_tuple_elements = [](auto&& tuple) {
		return std::apply(
		    [](auto&&... es) {
			    return std::tie(std::forward<decltype(es)>(es)...);
		    },
		    std::forward<decltype(tuple)>(tuple)
		);
	};

	namespace detail
	{
		template<auto... Xs>
		struct max;

		template<auto X>
		struct max<X>
		{
			static constexpr auto value = X;
		};

		template<auto X, auto... Xs>
		struct max<X, Xs...>
		{
			static constexpr auto value = std::max(X, detail::max<Xs...>::value);
		};
	}

	template<auto... Xs>
	using max_t = detail::max<Xs...>;

	template<auto... Xs>
	constexpr static auto max_v = detail::max<Xs...>::value;

	template<class T>
	concept is_span = requires {
		typename T::element_type;
		T::extent;
	} && std::same_as<std::remove_cvref_t<T>, std::span<typename T::element_type, T::extent>>;
}
