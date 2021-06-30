#pragma once

#include <type_traits>
#include <functional>

namespace te
{
	template<class... Ts>
	struct list;

	namespace detail
	{
		template<class E, class F>
		struct prepend;

		template<class E>
		struct prepend<E, void>
		{
			using val = list<E>;
		};

		template<class E, class... Args>
		struct prepend<E, list<Args...>>
		{
			using val = list<E, Args...>;
		};

		template<class E, class F>
		using prepend_t = typename prepend<E, F>::val;


		template<class E, class F>
		struct append;

		template<class E>
		struct append<E, void>
		{
			using val = list<E>;
		};

		template<class E, class... Args>
		struct append<E, list<Args...>>
		{
			using val = list<Args..., E>;
		};

		template<class E, class F>
		using append_t = typename append<E, F>::val;
	}


	template<class... Ts>
	struct list
	{
		static constexpr bool is_empty = true;
		static constexpr int size = 0;

		template<class E>
		using prepend_t = typename detail::prepend_t<E, list<Ts...>>;

		template<class E>
		using append_t = typename detail::append_t<E, list<Ts...>>;
	};

	template<class Head, class... Tail>
	struct list<Head, Tail...>
	{
		static constexpr bool is_empty = false;
		static constexpr int size = 1 + sizeof...(Tail);
		using head = Head;
		using tail = typename te::list<Tail...>;

		template<class E>
		using prepend_t = typename detail::prepend_t<E, list<Head, Tail...>>;

		template<class E>
		using append_t = typename detail::append_t<E, list<Head, Tail...>>;
	};

	template<class T>
	concept is_list = requires { T::is_empty; T::size; };

	template<template<class> class F, class L>
	struct map;

	template<template<class> class F, class... Ls>
	struct map<F, te::list<Ls...>>
	{
		using type = te::list<F<Ls>...>;
	};

	template<template<class> class F, class... Ls>
	struct map<F, std::tuple<Ls...>>
	{
		using type = std::tuple<F<Ls>...>;
	};

	template<template<class> class F, class L>
	using map_t = typename map<F, L>::type;

	template<template<class, class> class F, class L1, class L2>
	struct zip;

	template<template<class, class> class F, class... L1s, class... L2s>
	struct zip<F, std::tuple<L1s...>, std::tuple<L2s...>>
	{
		using type = std::tuple<F<L1s, L2s>...>;
	};



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
				typename te::detail::prepend_t<typename L::head, R>
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


	template<class L, class T>
	static constexpr bool contains_v = false;

	template<class... Ls, class T>
	static constexpr bool contains_v<te::list<Ls...>, T> = (std::same_as<Ls, T> || ...);


	template<class T>
	struct is_std_fun;

	template<class R, class... Args>
	struct is_std_fun<std::function<R(Args...)>> : std::true_type
	{
		using return_type = R;
		using arguments_list = list<Args...>;
	};

	template<class T>
	struct is_std_fun : std::false_type {};

	template<class T>
	inline constexpr bool is_std_fun_v = is_std_fun<T>::value;


	template<class T>
	struct is_c_fun;

	template<class R, class... Args>
	struct is_c_fun<R(*)(Args...)> : std::true_type
	{
		using return_type = R;
		using arguments_list = list<Args...>;
	};

	template<class T>
	struct is_c_fun : std::false_type {};

	template<class T>
	inline constexpr bool is_c_fun_v = is_c_fun<T>::value;


	template<class T>
	struct is_member_fun;

	template<class R, class B, class...Args>
	struct is_member_fun<R(B::*)(Args...)> : std::true_type
	{
		using return_type = R;
		using arguments_list = list<Args...>;
		using base_class = B;
	};

	template<class T>
	struct is_member_fun : std::false_type {};

	template<class T>
	inline constexpr bool is_member_fun_v = is_member_fun<T>::value;


	template<class, class = void>
	struct is_lambda_fun : std::false_type {};

	template<class T>
	struct is_lambda_fun<T, std::void_t<decltype(&T::operator())>>
	{
		static constexpr bool value = !is_std_fun<T>::value;
	};

	template<class T>
	inline constexpr bool is_lambda_fun_v = is_lambda_fun<T>::value;


	template<class T, class = void>
	struct deconstruct_fun;

	template<class T>
	struct deconstruct_fun<T, std::enable_if_t<is_c_fun_v<T>>>
	{
		using return_type = typename is_c_fun<T>::return_type;
		using arguments_list = typename is_c_fun<T>::arguments_list;
	};

	template<class T>
	struct deconstruct_fun<T, std::enable_if_t<is_std_fun_v<T>>>
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
		using arguments_list = list<Args...>;
	};


	template<class T>
	struct deconstruct_fun<T, std::enable_if_t<is_lambda_fun_v<T>>>
	{
		using return_type = typename deconstruct_lambda_member_fun<decltype(&T::operator())>::return_type;
		using arguments_list = typename deconstruct_lambda_member_fun<decltype(&T::operator())>::arguments_list;
	};

	template<class T>
	struct deconstruct_fun<T, std::enable_if_t<is_member_fun_v<T>>>
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

		template<class T, class List>
		struct test_fun;

		template<class T, class... Args>
		concept test_concept = requires (T t) { t(std::declval<Args>()...); };

		template<class T, class... Args>
		struct test_fun<T, te::list<Args...>>
		{
			constexpr static bool value = test_concept<T, Args...>;
		};
	}

	template<int I, class T>
	struct duplicate;

	template<int I, class T>
	using duplicate_t = typename duplicate<I, T>::type;

	template<class T>
	struct duplicate<1, T>
	{
		using type = te::list<T>;
	};

	template<int I, class T>
	struct duplicate
	{
		using type = typename duplicate<I - 1, T>::type::template prepend_t<T>;
	};

	template<class T, int I>
	concept number_of_arguments_is =
		impl::test_fun<T, duplicate_t<I, impl::nothing>>::value ||
		((requires (T t) { &T::operator(); }) && te::arguments_list_t<decltype(&T::operator())>::size == I) ||
		((te::is_c_fun_v<T> || te::is_lambda_fun_v<T>) && te::arguments_list_t<T>::size == I);
}
