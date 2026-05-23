#pragma once

#include <tepp/tepp.h>

namespace te
{
	template<class F>
	constexpr decltype(auto) invoke(F&& f) {
		if constexpr (std::same_as<te::return_type_t<F>, void>) {
			f();
		}
		else {
			return f();
		}
	}
}
