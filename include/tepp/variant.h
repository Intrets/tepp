#pragma once

#include <variant>

namespace te
{
	template<class... Ts>
	using variant = std::variant<Ts...>;

	namespace detail
	{
		template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
		template<class... Ts> overloaded(Ts...)->overloaded<Ts...>;
	}

	template<class V, class... Visitors>
	constexpr auto visit(V&& v, Visitors&&... visitors) {
		return std::visit(detail::overloaded{ std::forward<Visitors>(visitors)... }, std::forward<V>(v));
	}
}