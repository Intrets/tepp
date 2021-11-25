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

#include <cstdint>
#include <type_traits>
#include <concepts>

namespace te
{
	template<class Enum, class T = std::underlying_type_t<Enum>>
	struct enum_bitflags
	{
	private:
		T data{};
		enum_bitflags(T data_) : data(data_) {};

	public:
		enum_bitflags() = default;
		enum_bitflags(Enum e) {
			this->data = static_cast<T>(e);
		}
		~enum_bitflags() = default;

		constexpr enum_bitflags operator&(enum_bitflags other) const {
			return enum_bitflags{ this->data & other.data };
		}

		constexpr enum_bitflags& operator&=(enum_bitflags other) {
			this->data &= other.data;
			return *this;
		}

		constexpr enum_bitflags operator|(enum_bitflags other) const {
			return enum_bitflags{ this->data | other.data };
		}

		constexpr enum_bitflags& operator|=(enum_bitflags other) {
			this->data |= other.data;
			return *this;
		}

		constexpr enum_bitflags operator^(enum_bitflags other) const {
			return enum_bitflags{ this->data ^ other.data };
		}

		constexpr enum_bitflags& operator^=(enum_bitflags other) {
			this->data ^= other.data;
			return *this;
		}

		constexpr enum_bitflags operator~() {
			return enum_bitflags{ ~this->data };
		}

		constexpr operator bool() const {
			return this->data != 0;
		}

		constexpr bool test(enum_bitflags other) {
			return this->data & other.data;
		}
	};
}
