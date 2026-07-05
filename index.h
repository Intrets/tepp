#pragma once

#include "tepp/misc.h"

namespace te
{
	template<class Phantom, class T = integer_t>
	struct index2
	{
		T value = 0;

		static index2 make(T v) {
			auto result = index2();
			result.value = v;
			return result;
		}

		index2() = default;
		~index2() = default;

		DEFAULT_COPY_MOVE(index2);

		operator T() const {
			return this->value;
		}

		auto operator<=>(index2 const&) const = default;

		index2 operator+(index2 right) const {
			return index2{ this->value + right.value };
		}

		index2 operator-(index2 right) const {
			return index2{ this->value - right.value };
		}

		index2& operator++() {
			++this->value;
			return *this;
		}

		index2 operator++(int) {
			return index2(this->value++);
		}

		index2& operator--() {
			--this->value;
			return *this;
		}

		index2 operator--(int) {
			return index2(this->value--);
		}
	};
}
