#pragma once

namespace te
{
	template<class F>
	struct defer
	{
		F f;

		defer() = delete;
		defer(F&& f_)
		    : f(std::forward<F>(f_)) {
		}
		~defer() {
			this->f();
		}
	};
}
