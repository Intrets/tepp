#include "tepp/integers.h"

namespace te
{
	static_assert(idivmod(0, 3) == std::pair(0, 0));

	static_assert(idivmod(14, 3) == std::pair(4, 2));
	static_assert(idivmod(13, 3) == std::pair(4, 1));
	static_assert(idivmod(12, 3) == std::pair(4, 0));
	static_assert(idivmod(11, 3) == std::pair(3, 2));
	static_assert(idivmod(10, 3) == std::pair(3, 1));

	static_assert(idivmod(-10, 3) == std::pair(-4, 2));
	static_assert(idivmod(-11, 3) == std::pair(-4, 1));
	static_assert(idivmod(-12, 3) == std::pair(-4, 0));
	static_assert(idivmod(-13, 3) == std::pair(-5, 2));
	static_assert(idivmod(-14, 3) == std::pair(-5, 1));
}
