#pragma once

#include <concepts>
#include <utility>

#include "tepp/misc.h"

namespace te
{
	template<std::invocable F>
	struct call_on_scope_exit
	{
		F f;

		NO_COPY_MOVE(call_on_scope_exit);

		call_on_scope_exit() = delete;
		call_on_scope_exit(F&& f_) : f(std::forward<F>(f_)) {}
		~call_on_scope_exit() {
			std::invoke(this->f);
		}
	};
}