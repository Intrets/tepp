#pragma once

#include <optional>
#include <variant>

#include "tepp/tepp.h"

namespace te
{
	template<class... Ts>
	using variant = std::variant<Ts...>;

	namespace detail
	{
		template<class... Ts>
		struct overloaded : Ts...
		{
			using Ts::operator()...;
		};
		template<class... Ts>
		overloaded(Ts...) -> overloaded<Ts...>;
	}

	template<class V, class... Visitors>
	constexpr auto visit(V&& v, Visitors&&... visitors) {
		return std::visit(detail::overloaded{ std::forward<Visitors>(visitors)... }, std::forward<V>(v));
	}

	template<class V, class Visitor, class T = te::head_t<te::arguments_list_t<Visitor>>>
	constexpr auto visit_if(V&& v, Visitor&& visitor) {
		using Return = std::invoke_result_t<Visitor, T>;

		if (auto ptr = std::get_if<std::remove_cvref_t<T>>(&v)) {
			if constexpr (std::same_as<Return, void>) {
				visitor(*ptr);
			}
			else {
				return std::make_optional<Return>(visitor(*ptr));
			}
		}
		else {
			if constexpr (std::same_as<Return, void>) {
			}
			else {
				return std::optional<Return>(std::nullopt);
			}
		}
	}
}