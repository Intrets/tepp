#pragma once

#include "integers.h"
#include "misc.h"
#include "tepp.h"

#include <array>
#include <ranges>

namespace te
{
	struct mdspan_offset_calculator
	{
	};

	template<class T, integer_t N>
	struct mdspan
	{
	private:
		T* data_;

		std::array<integer_t, N> dims;

	public:
		template<std::integral... Is>
		constexpr bool in_range(Is... is) const {
			integer_t i = 0;
			bool result = true;
			te::for_each(
			    [&](auto size) {
				    if (size >= this->dims[i]) {
					    result = false;
				    }

				    i++;
			    },
			    is...
			);
			return result;
		}

		template<std::integral I, std::same_as<I>... Is>
		constexpr integer_t calculate_offset(I i, Is... is) const {
			static_assert(sizeof...(Is) + 1 == N);

			integer_t index = 0;

			integer_t n = N - 1;

			for (auto s : std::array{ i, is... } | std::views::reverse) {
				index *= this->dims[n];
				index += static_cast<integer_t>(s);
				n--;
			}

			return index;
		}

		template<class Self, std::integral... Is>
		requires(!std::same_as<T, mdspan_offset_calculator>)
		constexpr auto&& operator[](this Self&& self, Is... is) {
			static_assert(sizeof...(Is) == N);

			auto index = self.calculate_offset(is...);

			return *(self.data_ + index);
		}

		DEFAULT_COPY_MOVE(mdspan);

		constexpr mdspan()
		    : data_(nullptr),
		      dims{} {
		}

		template<class R, std::integral... I>
		requires(!std::same_as<T, mdspan_offset_calculator>)
		constexpr mdspan(R&& r, I... dims_)
		    : data_(&r[0]),
		      dims{ dims_... } {
			static_assert(sizeof...(I) == N);
		}

		template<std::integral... I>
		requires(std::same_as<T, mdspan_offset_calculator>)
		constexpr mdspan(I... dims_)
		    : data_(nullptr),
		      dims{ dims_... } {
			static_assert(sizeof...(I) == N);
		}

		~mdspan() = default;
	};

	template<std::integral... Is>
	mdspan(Is...) -> mdspan<mdspan_offset_calculator, sizeof...(Is)>;

	template<class T, std::integral... Is>
	mdspan(T, Is...) -> mdspan<std::remove_reference_t<decltype(std::declval<T>()[0])>, sizeof...(Is)>;

}
