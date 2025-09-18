// render - A C++ OpenGL library
// Copyright (C) 2021 intrets

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

constexpr te::angle<float> operator ""_deg(long double angle) {
	return te::fromDegrees<float>(static_cast<float>(angle));
}

constexpr te::angle<float> operator ""_rad(long double angle) {
	return te::fromRadians<float>(static_cast<float>(angle));
}

constexpr te::angle<long double> operator ""_ddeg(long double angle) {
	return te::fromDegrees<long double>(angle);
}

constexpr te::angle<long double> operator ""_drad(long double angle) {
	return te::fromRadians<long double>(angle);
}
