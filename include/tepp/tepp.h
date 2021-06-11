#pragma once

#include <type_traits>
#include <functional>

namespace te
{
	struct ctype_base
	{
		static inline size_t t = 1;
	};

	template<class T>
	struct ctype : ctype_base
	{
		static inline size_t val = t++;
	};


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

	template<class L, class = void>
	struct remove_ref_and_const;

	template<class L>
	struct remove_ref_and_const<L, std::enable_if_t<L::is_empty>>
	{
		using type = L;
	};

	template<class A, class... Args>
	struct remove_ref_and_const<list<A, Args...>, void>
	{
		using type = typename detail::prepend_t<
			std::remove_const_t<std::remove_reference_t<A>>,
			typename remove_ref_and_const<list<Args...>>::type
		>;
	};

	template<class L>
	using remove_ref_and_const_t = typename remove_ref_and_const<L>::type;


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


	namespace detail
	{
		template<class L, class T>
		requires is_list<L>
		struct contains
		{
			constexpr static bool val() {
				if constexpr (L::is_empty) {
					return false;
				}
				else if constexpr (std::is_same_v<T, typename L::head>) {
					return true;
				}
				else {
					return detail::contains<typename L::tail, T>::val();
				}
			}
		};
	}

	template<class L, class T>
	static constexpr bool contains_v = detail::contains<L, T>::val();


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

	template<class T>
	using arguments_list_t = typename deconstruct_fun<T>::arguments_list;

	template<class T>
	using member_function_base_class_t = typename deconstruct_fun<T>::base_class;

	template<class T>
	inline constexpr bool is_any_fun_v = is_c_fun_v<T> || is_lambda_fun_v<T> || is_member_fun_v<T> || is_std_fun_v<T>;

}