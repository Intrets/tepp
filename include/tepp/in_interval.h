#pragma once

namespace te
{
	template<class T>
	bool in_interval(T const& val, T const& min, T const& max) {
		return min <= val && val <= max;
	}
}
