#pragma once

#include <limits>
#include <tepp/optional_ref.h>

namespace te
{
	enum class limit_finder_limit
	{
		min,
		max
	};

	template<class T, class F, auto limit_finder_limit = limit_finder_limit::min>
	struct limit_finder
	{
		static F constexpr get_starting_limit() {
			if constexpr (limit_finder_limit == limit_finder_limit::min) {
				return std::numeric_limits<F>::max();
			}
			else {
				return std::numeric_limits<F>::lowest();
			}
		}

		static bool compare(F const& f1, F const& f2) {
			if constexpr (limit_finder_limit == limit_finder_limit::min) {
				return f1 < f2;
			}
			else {
				return f2 > f1;
			}
		}

		T* result = nullptr;
		F limit = get_starting_limit();

		void run(T& object, F const& v) {
			if (compare(v, this->limit)) {
				this->limit = v;
				this->result = &object;
			}
		}

		te::optional_ref<T> get_result() {
			if (this->result == nullptr) {
				return te::nullopt;
			}
			else {
				return *this->result;
			}
		}
	};
}