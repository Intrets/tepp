#pragma once

#include <ranges>

#include "tepp/integers.h"

namespace te
{
	inline constexpr auto enumerate_range = [](auto&& range) {
		return std::views::zip(std::views::iota(0_i), range);
	};

	inline constexpr auto scale_range = [](auto&& range) {
		return std::views::zip(std::views::iota(0_i) | std::views::transform([&](auto i) { return static_cast<float>(i) / isize(range); }), range);
	};
}