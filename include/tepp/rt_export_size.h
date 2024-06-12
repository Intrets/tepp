#pragma once

#include <cstdint>
#include <cassert>

namespace te
{
	namespace rt_export_size
	{
		struct normal
		{
			int64_t size;
		};

		struct power_of_two
		{
			int64_t power;

			int64_t getSize() {
				assert(power < 64);
				return 1ULL << power;
			}

			static constexpr power_of_two at_least(int64_t size) {
				if (size <= 0) {
					return { 0 };
				}

				int64_t power = 0;

				size--;

				while (size) {
					size >>= 1;
					power++;
				}

				return { power };
			}
		};

		enum class type
		{
			normal,
			restricted
		};
	}
}
