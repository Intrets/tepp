#include <concepts>
#include <tuple>
#include <type_traits>
#include <ranges>

#include "tepp/tepp.h"

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

namespace detail
{
	template<class T>
	struct Type
	{
		using type = T;
	};
}

template<class T>
constexpr auto Type = detail::Type<T>();

template<class T>
using Type_t = detail::Type<T>;

template<class... Args>
constexpr auto type_tuple = std::make_tuple(Type<Args>...);

namespace detail
{
	template<class T>
	struct is_tuple;

	template<class... Args>
	struct is_tuple<std::tuple<Args...>> : std::true_type {};

	template<class T>
	struct is_tuple : std::false_type {};
}

template<class T>
concept tuple = detail::is_tuple<T>::value;

template<class... Args>
consteval auto map(auto f, std::tuple<Args...> t) {
	return Type<decltype(std::apply(
		[](auto... args) {
			return std::tuple<std::invoke_result_t<decltype(f), decltype(args)>...>{};
		},
		t)) > ;
}

constexpr auto if_f(bool b, auto v1, auto v2) {
	if constexpr (b) {
		return v1;
	}
	else {
		return v2;
	}
}

struct everything_deleted
{
	int x;

	//everything_deleted() = delete;
	//everything_deleted& operator=(everything_deleted const&) = delete;
	//everything_deleted& operator=(everything_deleted &&) = delete;
	//everything_deleted(everything_deleted const&) = delete;
	//everything_deleted(everything_deleted &&) = delete;
};

auto test_everything_deleted(everything_deleted by_value) {
	return by_value.x;
}

template<class... Args>
consteval auto filter(auto f, std::tuple<Args...> t) {
	return map(
		[=](auto x) {
			//return if_f(true, 1, 2);
			return 1;
			//constexpr auto y = std::invoke(f, x);
			//return f(x) ? std::make_tuple(x) : std::tuple<>();
			//if constexpr (f(1)) {
			//	return std::make_tuple(x);
			//}
			//else {
			//	return std::tuple<>();
			//}
		},
		t);

}


int main() {
	constexpr auto t = std::make_tuple(1, 2, 3, 4);

	constexpr auto r = std::make_tuple(Value<1>, Value<2>, Value<3>, Value<4>);

	constexpr auto t2 = map([]<auto x>(Value_t<x>) { return Value<x * 2>; }, r);
	constexpr auto t3 = [](auto r) {
		return map([]<auto x>(Value_t<x>) { return Value<x * 2>; }, r);
	};
	//constexpr auto t3 = [](auto r) {
	//	return map([]<auto x>(Value_t<x>) { return Value<x % 2 == 0>; }, r);
	//};
	constexpr auto t4 = t3(r);

	constexpr auto type_test = type_tuple<int, float, float, int>;

	constexpr auto type_test2 = map([]<class T>(Type_t<T>) {
		if constexpr (std::same_as<T, int>) {
			return 1;
		}
		else {
			return Type<T>;
		}
	}, type_test);

	using T = std::invoke_result_t<decltype(test_everything_deleted), everything_deleted>;

	rand();
	//List<1, 3, 4, 5> a;

}