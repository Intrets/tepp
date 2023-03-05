// tepp - A template library for C++
// Copyright (C) 2021 intrets

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

		constexpr bool operator==(enum_bitflags other) const {
			return this->data == other.data;
		}

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
			return !this->empty();
		}

		constexpr bool empty() const {
			return this->data == value_type{};
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

		constexpr value_type getData() const {
			return this->data;
		}
	};

	template<class T, class... Ts>
	enum_bitflags(T, Ts...)->enum_bitflags<T>;
}
