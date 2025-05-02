#include "tepp/time.h"

#include <chrono>

namespace te
{
	using clock = std::chrono::steady_clock;

	clock::time_point get_time_point(time_point t) {
		return clock::time_point(std::chrono::nanoseconds(t.data));
	}

	bool time_point::after(time_point other) const {
		return this->data > other.data;
	}

	bool time_point::before(time_point other) const {
		return this->data < other.data;
	}

	bool time_point::after_or_equal(time_point other) const {
		return this->data >= other.data;
	}

	bool time_point::before_or_equal(time_point other) const {
		return this->data <= other.data;
	}

	bool time_point::elapsed(duration d) const {
		return (*this).add(d).before(time_point::now());
	}

	duration time_point::duration_to(duration other) const {
		return duration{ this->data - other.data };
	}

	time_point time_point::add(duration d) const {
		return { this->data + d.data };
	}

	time_point& time_point::set_now() {
		*this = now();

		return *this;
	}

	time_point time_point::now() {
		return time_point{ std::chrono::duration_cast<std::chrono::nanoseconds>(clock::now().time_since_epoch()).count() };
	}

	duration duration::s(int64_t count) {
		return duration(count * 1'000'000'000);
	}

	duration duration::ms(int64_t count) {
		return duration(count * 1'000'000);
	}

	duration duration::us(int64_t count) {
		return duration(count * 1'000);
	}

	duration duration::ns(int64_t count) {
		return duration(count);
	}
}
