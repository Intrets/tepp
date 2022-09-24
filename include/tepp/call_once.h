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