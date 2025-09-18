#pragma once

#include "tepp/assert.h"
#include "tepp/integers.h"

namespace te
{
	namespace rt_export_size
	{
		struct normal
		{
			integer_t size;
		};

		struct power_of_two
		{
			integer_t power;

			integer_t getSize() {
				tassert(power < 64);
				return 1ULL << power;
			}

			static constexpr power_of_two at_least(integer_t size) {
				if (size <= 0) {
					return { 0 };
				}

				integer_t power = 0;

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
