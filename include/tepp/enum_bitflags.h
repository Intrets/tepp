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
	template<class Enum>
	struct enum_bitflags
	{
	private:
		using value_type = std::underlying_type_t<Enum>;

		value_type data{};
		explicit enum_bitflags(value_type data_) : data(data_) {
		};

	public:
		enum_bitflags() = default;

		template<std::same_as<Enum>... Enums>
		enum_bitflags(Enums... e) : enum_bitflags((static_cast<value_type>(e) | ...)) {
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

		constexpr enum_bitflags operator~() const {
			return enum_bitflags{ ~this->data };
		}

		constexpr operator bool() const {
			return this->data != value_type{};
		}

		constexpr bool test(enum_bitflags other) const {
			return this->data & other.data;
		}

		constexpr void set(Enum d) {
			*this |= enum_bitflags(d);
		}

		constexpr void clear(Enum d) {
			*this &= ~enum_bitflags(d);
		}
	};

	template<class T, class... Ts>
	enum_bitflags(T, Ts...)->enum_bitflags<T>;
}
