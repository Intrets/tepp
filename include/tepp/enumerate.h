#pragma once

#include <ranges>

namespace te
{
	inline constexpr auto enumerate_range = [](auto&& range){
		return std::views::zip(std::views::iota(0_i), range);
	};
}