// tepp - A template library for C++
// Copyright (C) 2022  Intrets
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include <algorithm>

namespace te
{
	template<size_t N>
	struct string_literal
	{
		char val[N];
		static constexpr auto size = N;

		constexpr string_literal(char const(&str)[N]) {
			std::copy_n(str, N, this->val);
		}
	};
}
