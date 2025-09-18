#pragma once

#include <ranges>
#include <utility>

#include "tepp/misc.h"

namespace te
{
	template<class F>
	struct adapt
	{
		F f;

		NO_COPY_MOVE(adapt);

		adapt() = delete;

		template<class G>
		adapt(G&& g)
		    : f(std::forward<G>(g)) {
		}

		~adapt() = default;

		template<std::ranges::viewable_range R>
		auto operator()(R&& r) const {
			return f(std::forward<R>(r));
		}

		template<std::ranges::viewable_range R>
		friend auto operator|(R&& r, adapt<F> const& self) {
			return self(std::forward<R>(r));
		}
	};

	template<class G>
	adapt(G&& g) -> adapt<G>;

	template<class R1>
	auto zip2(R1&& r1) {
		return adapt([r2 = std::move(r1)](auto&& r) {
			return std::views::zip(r2, std::forward<decltype(r)>(r));
		});
	}
}
