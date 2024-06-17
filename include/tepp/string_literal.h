// tepp - A template library for C++
// Copyright (C) 2022 intrets

#pragma once

#include <algorithm>

#include "tepp/integers.h"

namespace te
{
	template<integer_t N>
	struct string_literal
	{
		char val[N];
		static constexpr auto size = N;

		constexpr string_literal(char const (&str)[N]) {
			std::copy_n(str, N, this->val);
		}
	};
}
