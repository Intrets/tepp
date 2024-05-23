#pragma once

namespace te
{
	namespace rt_export_size
	{
		struct normal
		{
			size_t size;
		};

		struct power_of_two
		{
			size_t power;

			size_t getSize() {
				assert(power < 64);
				return 1ULL << power;
			}

			static constexpr power_of_two at_least(int64_t size) {
				if (size <= 0) {
					return { 0 };
				}

				size_t power = 0;

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
