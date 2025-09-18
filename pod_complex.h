#pragma once

#include <complex>

namespace te
{
	template<class T>
	struct pod_complex
	{
		T real_;
		T imag_;

		constexpr pod_complex() = default;
		constexpr pod_complex(double d)
		    : real_(static_cast<T>(d)),
		      imag_() {
		}
		constexpr pod_complex(std::complex<T> const& c)
		    : real_(c.real()),
		      imag_(c.imag()) {
		}

		constexpr std::complex<T> get_std_complex() const {
			return std::complex<T>(this->real(), this->imag());
		}

		constexpr bool operator==(pod_complex const& other) const {
			return this->get_std_complex() == other.get_std_complex();
		}

		constexpr pod_complex operator/(pod_complex const& other) const {
			return this->get_std_complex() / other.get_std_complex();
		}

		constexpr pod_complex& operator/=(pod_complex const& other) {
			*this = this->get_std_complex() / other.get_std_complex();
			return *this;
		}

		constexpr pod_complex operator*(pod_complex const& other) const {
			return this->get_std_complex() * other.get_std_complex();
		}

		constexpr pod_complex& operator*=(pod_complex const& other) {
			*this = this->get_std_complex() * other.get_std_complex();
			return *this;
		}

		constexpr pod_complex operator+(pod_complex const& other) const {
			return this->get_std_complex() + other.get_std_complex();
		}

		constexpr pod_complex& operator+=(pod_complex const& other) {
			*this = this->get_std_complex() + other.get_std_complex();
			return *this;
		}

		constexpr pod_complex operator-(pod_complex const& other) const {
			return this->get_std_complex() - other.get_std_complex();
		}

		constexpr pod_complex& operator-=(pod_complex const& other) {
			*this = this->get_std_complex() - other.get_std_complex();
			return *this;
		}

		constexpr pod_complex operator-() const {
			return -this->get_std_complex();
		}

		constexpr T real() const {
			return this->real_;
		}

		constexpr T imag() const {
			return this->imag_;
		}
	};
}

#define FREE_FUN(name) \
	template<class T> \
	auto name(te::pod_complex<T> const& c) { \
		return std::name(c.get_std_complex()); \
	}

FREE_FUN(abs);
FREE_FUN(sin);
FREE_FUN(cos);
FREE_FUN(exp);
FREE_FUN(tan);
FREE_FUN(asin);
FREE_FUN(acos);
FREE_FUN(sqrt);
FREE_FUN(log);
FREE_FUN(log10);

#undef FREE_FUN

template<class T>
auto pow(te::pod_complex<T> const& c, int e) {
	return std::pow(c.get_std_complex(), e);
}

template<class T>
auto pow(te::pod_complex<T> const& c, te::pod_complex<T> const& e) {
	return std::pow(c.get_std_complex(), e.get_std_complex());
}

template<class T>
auto pow(te::pod_complex<T> const& c, T const& e) {
	return std::pow(c.get_std_complex(), e);
}

template<class T, class R>
auto pow(R const& b, te::pod_complex<T> const& c) {
	return std::pow(b, c.get_std_complex());
}