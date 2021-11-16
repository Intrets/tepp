#pragma once

#include <type_traits>
#include <functional>
#include <tuple>

namespace te
{
	template<class... Args>
	using list_type = std::tuple<Args...>;

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

	template<list L>
	using head_t = detail::head<L>::type;

	template<list L, class T>
	using prepend_t = detail::prepend<L, T>::type;

	template<list L, class T>
	using append_t = detail::append<L, T>::type;

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

#define Decltype(X) te::Type<decltype(X)>

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
		return[]<class S>(S) {
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

	namespace detail
	{
		template<size_t N>
		struct string
		{
			constexpr string(const char(&str)[N]) {
				std::copy_n(str, N, value);
			}

			char value[N];
			static constexpr auto size = N;
		};
	}

#define String(X) Value<detail::string(X)>

	namespace detail
	{
		template<int I, class L, class R>
		struct reverse;

		template<class L, class R>
		struct reverse<0, L, R>
		{
			using value = R;
		};

		template<int I, class L, class R>
		struct reverse
		{
			using value = typename reverse<
				I - 1,
				typename L::tail,
				typename te::detail::prepend<te::detail::head<L>, R>
				//typename te::detail::prepend_t<typename L::head, R>
			>::value;
		};
	}

	template<class T>
	struct reverse
	{
		using value = typename detail::reverse<T::size, T, void>::value;
	};

	template<class T>
	using reverse_t = typename reverse<T>::value;

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

	template<class List>
	using enumerate_t = typename enumerate<List, std::make_index_sequence<List::size>>::type;


	template<class L, class T>
	static constexpr bool contains_v = false;

	template<template<class...> class L, class... Args, class T>
	static constexpr bool contains_v<L<Args...>, T> = (std::same_as<Args, T> || ...);


	template<class T>
	struct is_std_fun;

	template<class R, class... Args>
	struct is_std_fun<std::function<R(Args...)>> : std::true_type
	{
		using return_type = R;
		using arguments_list = list_type<Args...>;
	};

	template<class T>
	struct is_std_fun : std::false_type {};

	template<class T>
	inline constexpr bool is_std_fun_v = is_std_fun<T>::value;

	template<class T>
	concept std_fun = is_std_fun_v<T>;


	template<class T>
	struct is_c_fun;

	template<class R, class... Args>
	struct is_c_fun<R(*)(Args...)> : std::true_type
	{
		using return_type = R;
		using arguments_list = list_type<Args...>;
	};

	template<class T>
	struct is_c_fun : std::false_type {};

	template<class T>
	inline constexpr bool is_c_fun_v = is_c_fun<T>::value;

	template<class T>
	concept c_fun = is_c_fun_v<T>;


	template<class T>
	struct is_member_fun;

	template<class R, class B, class...Args>
	struct is_member_fun<R(B::*)(Args...)> : std::true_type
	{
		using return_type = R;
		using arguments_list = list_type<Args...>;
		using base_class = B;
	};

	template<class T>
	struct is_member_fun : std::false_type {};

	template<class T>
	inline constexpr bool is_member_fun_v = is_member_fun<T>::value;

	template<class T>
	concept member_fun = is_member_fun_v<T>;


	template<class, class = void>
	struct is_lambda_fun : std::false_type {};

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
	struct deconstruct_lambda_member_fun<R(B::*)(Args...) const>
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
	using member_function_base_class_t = typename deconstruct_fun<T>::base_class;

	template<class T>
	inline constexpr bool is_any_fun_v = is_c_fun_v<T> || is_lambda_fun_v<T> || is_member_fun_v<T> || is_std_fun_v<T>;

	namespace impl
	{
		struct nothing {};

		template<class T, class... Args>
		concept number_arguments_concept = requires (T t) { t(std::declval<Args>()...); };

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
		using type = prepend_t<next, T>;
	};

	template<class T, int I>
	concept number_of_arguments_is =
		impl::number_arguments_test<T, replicate_t<I, impl::nothing>>::value ||
		((requires (T t) { &T::operator(); }) && te::arguments_list_t<decltype(&T::operator())>::size == I) ||
		((te::is_c_fun_v<T> || te::is_lambda_fun_v<T>) && te::arguments_list_t<T>::size == I);
}