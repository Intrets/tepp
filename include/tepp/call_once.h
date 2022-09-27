// tepp - A template library for C++
// Copyright (C) 2022 intrets

#pragma once

#include <utility>

namespace te
{
	namespace detail
	{
		template<class F>
		struct call_once_t
		{
			call_once_t() = delete;
			call_once_t(F&& f) {
				std::forward<F>(f)();
			}
			~call_once_t() = default;
		};
	}

	template<class F>
	static void call_once(F&& f) {
		static auto once = detail::call_once_t(std::forward<F>(f));
	}
}
