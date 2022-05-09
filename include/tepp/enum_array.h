// tepp - A template library for C++
// Copyright (C) 2022  Intrets
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

#include <array>
#include <cassert>
#include <ranges>
#include <algorithm>
#include <optional>

namespace te
{
	template<class EnumType, class T, size_t size = static_cast<size_t>(EnumType::MAX)>
	struct enum_array
	{
	private:
		std::array<T, size> data{};

	public:
		constexpr T& operator[](EnumType i) {
			assert(static_cast<size_t>(i) < size);
			return this->data[static_cast<size_t>(i)];
		}

		constexpr T const& operator[](EnumType i) const {
			assert(static_cast<size_t>(i) < size);
			return this->data[static_cast<size_t>(i)];
		}

		template<class S>
		std::optional<EnumType> getEnum(S&& value) const {
			for (size_t i = 0; i < size; i++) {
				auto e = static_cast<EnumType>(i);

				if (this->operator[](e) == value) {
					return e;
				}
			}

			return std::nullopt;
		}

		constexpr enum_array() = default;

		template<class... Args>
		constexpr enum_array(Args&&... args) : data{ args... } {
		}

		using A = std::array<T, size>;
		using const_iterator = typename A::const_iterator;
		using iterator = typename A::iterator;

		template<EnumType start, size_t N>
		constexpr void write(std::span<T, N> values) {
			if constexpr (N == std::dynamic_extent) {
				constexpr auto end = std::min(size, N);
				assert(end == N);

				std::copy(values.begin(), values.begin() + end, this->data.begin());
			}
			else {
				static_assert(static_cast<size_t>(start) + N <= size, "Too many values in span");

				std::copy(values.begin(), values.end(), this->data.begin());
			}
		}

		template<EnumType begin, EnumType end>
		constexpr auto getSpan() {
			constexpr auto beginI = static_cast<size_t>(begin);
			constexpr auto endI = static_cast<size_t>(end) + 1;
			constexpr auto N = endI - beginI;
			static_assert(N > 0);
			static_assert(N <= size);

			return std::span<T, N>{ this->data.begin() + beginI, this->data.begin() + endI };
		}

		template<EnumType begin>
		constexpr auto getSpan(size_t N) {
			constexpr auto beginI = static_cast<size_t>(begin);
			auto endI = beginI + N;
			assert(N <= size);

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
	};
}
