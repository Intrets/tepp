// render - A C++ OpenGL library
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

#include <concepts>

#include <numbers>

#include <tepp/numeric_type.h>

namespace te
{
	struct angle_type;

	template<std::floating_point T>
	using angle = numeric_type<T, angle_type>;

	template<std::floating_point T>
	constexpr static angle<T> fromRadians(T a) {
		return angle<T>::make(a * 2 * std::numbers::pi_v<T>);
	}

	template<std::floating_point T>
	constexpr static angle<T> fromDegrees(T a) {
		constexpr auto one_over_360 = T(0.002777777777777777777777777778);
		return angle<T>::make(a * one_over_360 * 2 * std::numbers::pi_v<T>);
	}
}

constexpr te::angle<float> operator "" _deg(long double angle) {
	return te::fromDegrees<float>(static_cast<float>(angle));
}

constexpr te::angle<float> operator "" _rad(long double angle) {
	return te::fromRadians<float>(static_cast<float>(angle));
}

constexpr te::angle<long double> operator "" _ddeg(long double angle) {
	return te::fromDegrees<long double>(angle);
}

constexpr te::angle<long double> operator "" _drad(long double angle) {
	return te::fromRadians<long double>(angle);
}
