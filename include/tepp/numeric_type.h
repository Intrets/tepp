// tepp - a template library for c++
// Copyright (C) 2021 intrets

#pragma once

#include <concepts>

namespace te
{
	template<class M, class T>
	concept has_modulus_operation = requires(T t, M m) { t% m; };

	template<class T>
	concept has_increment_operations = requires(T t) { t++; ++t; t--; --t; };

	template<class T, class Label>
	struct numeric_type
	{
		using S = numeric_type<T, Label>;

	protected:
		T data{};

		constexpr numeric_type(T t) : data(t) {};

	public:
		T get() {
			return this->data;
		}

		constexpr numeric_type() = default;
		~numeric_type() = default;

		constexpr static numeric_type<T, Label> make(T t = {}) {
			return numeric_type<T, Label>(t);
		}

		constexpr bool operator<(S other) const {
			return this->data < other.data;
		}

		constexpr bool operator>(S other) const {
			return this->data > other.data;
		}

		// Mod

		template<has_modulus_operation<T> M>
		constexpr S operator%(M&& m) const {
			return { .data = this->data % m };
		}

		template<has_modulus_operation<T> M>
		S& operator%=(M&& m) {
			this->data %= m;
			return *this;
		}

		// Increments

		template<has_increment_operations = T>
		S& operator++() {
			++this->data;
			return *this;
		}

		template<has_increment_operations = T>
		S operator++(int) {
			auto old = *this;
			this->data++;
			return old;
		}

		// Decrements

		template<has_increment_operations = T>
		S& operator--() {
			--this->data;
			return *this;
		}

		template<has_increment_operations = T>
		S operator--(int) {
			auto old = *this;
			this->data--;
			return old;
		}

		// Addition

		constexpr S operator+(S other) const {
			return S(this->data + other.data);
		}

		S& operator+=(S other) {
			this->data += other.data;
			return *this;
		}

		// Substraction

		constexpr S operator-() const {
			return S(-this->data);
		}

		constexpr S operator-(S other) const {
			return S(this->data - other.data);
		}

		S& operator-=(S other) {
			this->data -= other.data;
			return *this;
		}

		// Division

		constexpr T operator/(S other) const {
			return this->data / other.data;
		}

		template<class N>
		constexpr S operator/(N n) const {
			return S(this->data / n);
		}

		template<class N>
		S& operator/=(N n) {
			this->data /= n;
			return *this;
		}

		// Multiplication

		constexpr T operator*(S other) const {
			return this->data * other.data;
		}

		template<class N>
		constexpr S operator*(N n) const {
			return S(this->data * n);
		}

		template<class N>
		S& operator*=(N n) {
			this->data *= n;
			return *this;
		}

		// Equality

		constexpr bool operator==(numeric_type<T, Label> other) const {
			return this->data == other.data;
		}

		constexpr bool operator!=(numeric_type<T, Label> other) const {
			return !(*this == other);
		}
	};

	template<std::floating_point T, class Label, class N>
	numeric_type<T, Label> operator*(N n1, numeric_type<T, Label> n2) {
		return numeric_type<T, Label>::make(n2.get() * n1);
	}
}
