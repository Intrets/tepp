#pragma once

#include <bit>
#include <bitset>
#include <concepts>
#include <cstdint>
#include <type_traits>

#include <tepp/integers.h>

namespace te
{
	template<class Enum>
	concept has_max = requires(Enum) { Enum::MAX; };

	template<has_max Enum>
	struct enum_bitflags
	{
	private:
		static constexpr integer_t N = static_cast<integer_t>(Enum::MAX);

		using value_type = std::bitset<N>;

		value_type data{};

		constexpr explicit enum_bitflags(value_type data_)
		    : data(data_){};

	public:
		constexpr enum_bitflags() = default;

		template<std::same_as<Enum>... Enums>
		constexpr enum_bitflags(Enums... e) {
			(this->data.set(static_cast<integer_t>(e)), ...);
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
			return this->data.none();
		}

		constexpr bool test(enum_bitflags other) const {
			return (this->data & other.data).any();
		}

		constexpr void set(Enum d) {
			this->data.set(static_cast<integer_t>(d), true);
		}

		constexpr void set(Enum d, bool b) {
			if (b) {
				this->set(d);
			}
			else {
				this->clear(d);
			}
		}

		constexpr void clear(Enum d) {
			this->data.set(static_cast<integer_t>(d), false);
		}

		constexpr value_type getData() const {
			return this->data;
		}
	};

	template<class T, class... Ts>
	enum_bitflags(T, Ts...) -> enum_bitflags<T>;
}
