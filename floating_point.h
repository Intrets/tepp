#pragma once

#include <concepts>
#include <cmath>

namespace te
{
	// Check if (1/f)^2 results in finite value. Squared to have some headroom.
	template<std::floating_point F>
	constexpr bool floating_division_check(F f) {
		if constexpr (std::same_as<F, float>) {
			return std::abs(f) > 1e-19;
		}
		else if constexpr (std::same_as<F, double>) {
			return std::abs(f) > 1e-150;
		}
	}

	// Check if (1/f)^2 results in finite value. Squared to have some headroom.
	template<std::floating_point F>
	constexpr bool floating_zero_check(F f) {
		return floating_division_check(f);
	}
}
