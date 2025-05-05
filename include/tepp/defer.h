#pragma once

#include <utility>

namespace te
{
	template<class F>
	struct [[nodiscard]] defer
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
