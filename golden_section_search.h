#pragma once

#include <numbers>

#include "integers.h"

namespace te
{
	template<class T>
	struct golden_section_search_result
	{
		T scale;
		T value;
	};

	template<std::floating_point T, class F>
	constexpr golden_section_search_result<T> golden_section_search(T s1, T s4, F&& f) {
		integer_t limit = std::same_as<T, float> ? 35 : 77;

		auto constexpr inv_phi = T(1.0) / std::numbers::phi_v<T>;

		auto s2 = s4 - (s4 - s1) * inv_phi;
		auto s3 = s1 + (s4 - s1) * inv_phi;

		auto v1 = f(s1);
		auto v2 = f(s2);
		auto v3 = f(s3);
		auto v4 = f(s4);

		for (integer_t j = 0; j < limit; j++) {
			if (v2 > v3) {
				s4 = s3;
				v4 = v3;

				s3 = s2;
				v3 = v2;

				s2 = s4 - (s4 - s1) * inv_phi;
				v2 = f(s2);
			}
			else {
				s1 = s2;
				v1 = v2;

				s2 = s3;
				v2 = v3;

				s3 = s1 + (s4 - s1) * inv_phi;
				v3 = f(s3);
			}
		}

		return {
			.scale = (s1 + s4) * T(0.5),
			.value = (v1 + v4) * T(0.5),
		};
	}
}
