#pragma once

#include <optional>

namespace te
{
	template<class T, class F>
	inline bool optional_equal_with(std::optional<T> const& left, std::optional<T> const& right, F&& cmp) {
		if (left.has_value() != right.has_value()) {
			return false;
		}
		if (!left.has_value()) {
			return false;
		}
		return cmp(left.value(), right.value());
	}
}
