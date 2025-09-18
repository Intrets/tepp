#pragma once

namespace te
{
	template<class T>
	struct literal
	{
		T value;

		constexpr literal(T value_) : value(value_) {}
	};
}
