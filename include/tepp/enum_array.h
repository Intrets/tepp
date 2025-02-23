// tepp - A template library for C++
// Copyright (C) 2022 intrets

#pragma once

#include <algorithm>
#include <array>
#include <optional>
#include <ranges>

#include <tepp/integers.h>

namespace te
{
	template<class EnumType, class T, integer_t size = static_cast<integer_t>(EnumType::MAX)>
	struct enum_array
	{
		using enum_type = EnumType;

		std::array<T, size> data{};

		integer_t getSize() const noexcept {
			return size;
		}

		constexpr T& operator[](EnumType i) {
			tassert(static_cast<integer_t>(i) < size);
			return this->data[static_cast<integer_t>(i)];
		}

		constexpr T const& operator[](EnumType i) const {
			tassert(static_cast<integer_t>(i) < size);
			return this->data[static_cast<integer_t>(i)];
		}

		template<class S>
		std::optional<EnumType> getEnum(S&& value) const {
			for (integer_t i = 0; i < size; i++) {
				auto e = static_cast<EnumType>(i);

				if (this->operator[](e) == value) {
					return e;
				}
			}

			return std::nullopt;
		}

		constexpr enum_array() = default;

		template<class... Args>
		constexpr enum_array(Args&&... args)
		    : data{ args... } {
		}

		constexpr enum_array(std::initializer_list<std::pair<EnumType, T>> init) {
			for (auto [e, val] : init) {
				(*this)[e] = val;
			}
		}

		constexpr enum_array(std::initializer_list<T> init) {
			integer_t i = 0;
			for (auto val : init) {
				this->data[i] = val;

				i++;
			}
		}

		using A = std::array<T, size>;
		using const_iterator = typename A::const_iterator;
		using iterator = typename A::iterator;

		template<EnumType start, integer_t N>
		constexpr void write(std::span<T, N> values) {
			if constexpr (N == std::dynamic_extent) {
				constexpr auto end = std::min(size, N);
				tassert(end == N);

				std::copy(values.begin(), values.begin() + end, this->data.begin());
			}
			else {
				static_assert(static_cast<integer_t>(start) + N <= size, "Too many values in span");

				std::copy(values.begin(), values.end(), this->data.begin());
			}
		}

		template<EnumType begin, EnumType end>
		constexpr auto getSpan() {
			constexpr auto beginI = static_cast<integer_t>(begin);
			constexpr auto endI = static_cast<integer_t>(end) + 1;
			constexpr auto N = endI - beginI;
			static_assert(N > 0);
			static_assert(N <= size);

			return std::span<T, N>{ this->data.begin() + beginI, this->data.begin() + endI };
		}

		template<EnumType begin>
		constexpr auto getSpan(integer_t N) {
			constexpr auto beginI = static_cast<integer_t>(begin);
			auto endI = beginI + N;
			tassert(N <= size);

			return std::span<T>{ this->data.begin() + beginI, this->data.begin() + endI };
		}

		constexpr void fill(T const& v) noexcept {
			data.fill(v);
		}

		constexpr const_iterator begin() const noexcept {
			return data.begin();
		}

		constexpr iterator begin() noexcept {
			return data.begin();
		}

		constexpr const_iterator end() const noexcept {
			return data.end();
		}

		constexpr iterator end() noexcept {
			return data.end();
		}

		template<class F>
		constexpr void forEach(F&& f) const {
			for (integer_t i = 0; i < size; i++) {
				auto e = static_cast<EnumType>(i);

				f(e, this->operator[](e));
			}
		}

		template<class F>
		constexpr void forEach(F&& f) {
			for (integer_t i = 0; i < size; i++) {
				auto e = static_cast<EnumType>(i);

				f(e, this->operator[](e));
			}
		}

		constexpr std::optional<EnumType> find(T const& value) const {
			for (integer_t i = 0; i < size; i++) {
				auto e = static_cast<EnumType>(i);

				if (this->operator[](e) == value) {
					return e;
				};
			}

			return std::nullopt;
		}

		auto getRawPtr() noexcept {
			return this->data.data();
		}
	};
}
