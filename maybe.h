#pragma once

#include "tepp/optional_ref.h"

#include <functional>
#include <optional>

namespace te
{
	template<class F>
	auto maybe(bool b, F&& f) {
		using Return = std::invoke_result_t<F>;
		if constexpr (std::is_reference_v<Return>) {
			using FullType = optional_ref<std::remove_reference_t<Return>>;
			if (b) {
				return FullType(std::invoke(f));
			}
			else {
				return FullType(nullopt);
			}
		}
		else {
			using FullType = std::optional<std::remove_reference_t<Return>>;
			if (b) {
				return std::make_optional(std::invoke(f));
			}
			else {
				return FullType(std::nullopt);
			}
		}
	}
}
