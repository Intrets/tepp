// tepp - A template library for C++
// Copyright (C) 2021  Intrets
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

#include <array>
#include <cassert>

namespace te
{
	template<class EnumType, class T, size_t size = static_cast<size_t>(EnumType::MAX)>
	struct enum_array
	{
	private:
		std::array<T, size> data{};

	public:
		constexpr T& operator[](EnumType i) {
			assert(static_cast<size_t>(i) < size);
			return this->data[static_cast<size_t>(i)];
		}

		using A = std::array<T, size>;

		constexpr A::const_iterator begin() const noexcept {
			return data.begin();
		}

		constexpr A::iterator begin() noexcept {
			return data.begin();
		}

		constexpr A::const_iterator end() const noexcept {
			return data.end();
		}

		constexpr A::iterator end() noexcept {
			return data.end();
		}
	};
}
