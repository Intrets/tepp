#include "tepp/bytes_number.h"

namespace te
{
	KB::operator bytes_number() const {
		return bytes_number{ static_cast<integer_t>(bytes_number::kB_factor * this->value) };
	}

	MB::operator bytes_number() const {
		return bytes_number{ static_cast<integer_t>(bytes_number::MB_factor * this->value) };
	}

	GB::operator bytes_number() const {
		return bytes_number{ static_cast<integer_t>(bytes_number::GB_factor * this->value) };
	}

	TB::operator bytes_number() const {
#if INTEGER_BITS >= 64
		return bytes_number{ static_cast<integer_t>(bytes_number::TB_factor * this->value) };
#else
        return bytes_number{0};
#endif
	}

	KB bytes_number::KB() const {
		return { this->value / static_cast<double>(kB_factor) };
	}

	MB bytes_number::MB() const {
		return { this->value / static_cast<double>(MB_factor) };
	}

	GB bytes_number::GB() const {
		return { this->value / static_cast<double>(GB_factor) };
	}

	TB bytes_number::TB() const {
#if INTEGER_BITS >= 64
		return { this->value / static_cast<double>(TB_factor) };
#else
        return {};
#endif
	}

	bytes_number bytes_number::operator+(bytes_number other) const {
		return { this->value + other.value };
	}

	bytes_number& bytes_number::operator+=(bytes_number other) {
		this->value += other.value;
		return *this;
	}

	bytes_number bytes_number::operator-(bytes_number other) const {
		return { this->value - other.value };
	}

	bytes_number& bytes_number::operator-=(bytes_number other) {
		this->value -= other.value;
		return *this;
	}
}
