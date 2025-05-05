#pragma once

#include <utility>

namespace te
{
	namespace detail
	{
		template<class F>
		struct defer
		{
			F f;

			defer(defer&&) = delete;
			defer& operator=(defer&&) = delete;

			defer(defer const&) = delete;
			defer& operator=(defer const&) = delete;

			defer() = delete;
			defer(F&& f_)
			    : f(std::forward<F>(f_)) {
			}
			~defer() {
				this->f();
			}
		};
	}

	template<class F>
	[[nodiscard]] detail::defer<F> defer(F&& f) {
		return detail::defer<F>(std::forward<F>(f));
	}
}
