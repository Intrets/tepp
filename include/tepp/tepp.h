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

		template<class... Args1>
		struct prepend<te::list<Args1...>, void>
		{
			using val = te::list<Args1...>;
		};

		template<class E>
		struct prepend<E, void>
		{
			using val = list<E>;
		};

		template<class... Args1, class... Args2>
		struct prepend<te::list<Args1...>, te::list<Args2...>>
		{
			using val = te::list<Args1..., Args2...>;
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

		template<class... Args1>
		struct append<te::list<Args1...>, void>
		{
			using val = te::list<Args1...>;
		};

		template<class E>
		struct append<E, void>
		{
			using val = list<E>;
		};

		template<class... Args1, class... Args2>
		struct append<te::list<Args1...>, te::list<Args2...>>
		{
			using val = te::list<Args2..., Args1...>;
		};

		template<class E, class... Args>
		struct append<E, list<Args...>>
		{
			using val = list<Args..., E>;
		};

		template<class E, class F>
		using append_t = typename append<E, F>::val;
	}

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

	namespace detail
	{
		template<class T>
		struct is_struct
		{
			template<class R>
			struct type
			{
				constexpr static bool value = std::same_as<std::remove_cvref_t<R>, T>;
			};
		};

		template<class WF>
		struct not_struct;

		template<template<class> class F>
		struct not_struct<te::wf<F>>
		{
			template<class T>
			using type = std::negation<F<T>>;
		};
	}


	template<class T>
	using is_ = te::wf<detail::is_struct<T>::template type>;

	template<class WF>
	using not_ = te::wf<detail::not_struct<WF>::template type>;


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

	template<template<class, class> class F, class... L1s, class... L2s>
	struct zip<F, te::list<L1s...>, te::list<L2s...>>
	{
		using type = te::list<F<L1s, L2s>...>;
	};

	template<template<class, class> class F, class L1, class L2>
	using zip_t = typename zip<F, L1, L2>::type;


	template<class T>
	struct to_list;

	template<class... Args>
	struct to_list<te::list<Args...>>
	{
		using type = te::list<Args...>;

	};

	template<class... Args>
	struct to_list<std::tuple<Args...>>
	{
		using type = te::list<Args...>;
	};

	template<class T, int I>
	struct enumeration
	{
		using type = T;
		static constexpr size_t index = I;
	};


	template<class List, class Is>
	struct enumerate;

	template<>
	struct enumerate<te::list<>, std::index_sequence<>>
	{
		using type = te::list<>;
	};

	template<class... Args, int... Is>
	struct enumerate<te::list<Args...>, std::index_sequence<Is...>>
	{
		using type = te::list<enumeration<Args, Is>...>;
	};

	template<class List>
	using enumerate_t = typename enumerate<List, std::make_index_sequence<List::size>>::type;

	template<class WF, class List>
	struct filter;

	template<template<class> class P, class Arg>
	struct filter<wf<P>, te::list<Arg>>
	{
		using type = std::conditional_t<P<Arg>::value,
			te::list<Arg>,
			te::list<>>;
	};

	template<template<class> class P, class Arg, class... Args>
	struct filter<wf<P>, te::list<Arg, Args...>>
	{
		using next = typename filter<wf<P>, te::list<Args...>>::type;
		using type = std::conditional_t<P<Arg>::value,
			typename next::template prepend_t<Arg>,
			next>;
	};

	template<class WF, class List>
	using filter_t = typename filter<WF, List>::type;

	template<class WF, class List>
	struct all;

	template<class WF, class... Args>
	struct all<WF, te::list<Args...>>
	{
		static constexpr bool value = (WF::template value<Args> && ...);
	};

	template<class WF, class List>
	constexpr bool all_v = all<WF, List>::value;

	template<class List>
	struct to_tuple;

	template<class... Args>
	struct to_tuple<te::list<Args...>>
	{
		using type = std::tuple<Args...>;
	};

	template<class List>
	using to_tuple_t = typename to_tuple<List>::type;


	template<int I, class Tuple>
	struct tuple_type_at
	{
		using type = decltype(std::get<I>(std::declval<Tuple>()));
	};

	template<int I, class Tuple>
	using tuple_type_at_t = typename tuple_type_at<I, Tuple>::type;


	template<class WF>
	struct inspect_type;

	template<template<class> class P>
	struct inspect_type<wf<P>>
	{
		template<class T>
		using type = P<typename T::type>;
	};

	template<class WF>
	using inspect_type_ = wf<inspect_type<WF>::template type>;


	namespace detail
	{
		template<class Is, class Tuple>
		struct filter_tuple_detail;

		template<class... Is, class Tuple>
		struct filter_tuple_detail<te::list<Is...>, Tuple>
		{
			constexpr static auto run(Tuple&& tuple) {
				return std::forward_as_tuple(std::get<Is::index>(tuple)...);
			}
		};
	}

	template<class WF>
	struct filter_tuple
	{
		template<class... Args>
		constexpr static auto run(std::tuple<Args...>&& tuple) {
			using enumerated_list = enumerate_t<te::list<Args...>>;
			using yes = filter_t<inspect_type_<WF>, enumerated_list>;

			return detail::filter_tuple_detail<yes, std::tuple<Args...>>::run(std::forward<decltype(tuple)>(tuple));
		}
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
	concept std_fun = is_std_fun_v<T>;


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
	concept c_fun = is_c_fun_v<T>;


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
		using arguments_list = list<Args...>;
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

		template<class T, class List>
		struct number_arguments_test;

		template<class T, class... Args>
		struct number_arguments_test<T, te::list<Args...>>
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
		using type = te::list<>;
	};

	template<class T>
	struct replicate<1, T>
	{
		using type = te::list<T>;
	};

	template<int I, class T>
	struct replicate
	{
		using type = typename replicate<I - 1, T>::type::template prepend_t<T>;
	};

	template<class T, int I>
	concept number_of_arguments_is =
		impl::number_arguments_test<T, replicate_t<I, impl::nothing>>::value ||
		((requires (T t) { &T::operator(); }) && te::arguments_list_t<decltype(&T::operator())>::size == I) ||
		((te::is_c_fun_v<T> || te::is_lambda_fun_v<T>) && te::arguments_list_t<T>::size == I);
}
