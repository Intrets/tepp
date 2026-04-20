#pragma once

#include "tepp/misc.h"
#include <array>
#include <atomic>

#include <tepp/assert.h>

namespace te
{
	template<class T>
	struct rt_export_buffered_value
	{
		T* rt_value = &buffers[0];

		std::atomic<T*> to_rt = &buffers[1];
		std::atomic<T*> from_rt = nullptr;

		T* non_rt_value = &buffers[2];

		std::array<T, 3> buffers{};

		T& read() {
			if (this->to_rt.load() == nullptr) {
				auto newValue = this->from_rt.exchange(nullptr);

				if (newValue != nullptr) {
					[[maybe_unused]]
					auto old = this->to_rt.exchange(this->non_rt_value);
					tassert(old == nullptr);
					this->non_rt_value = newValue;
				}
			}

			return *this->non_rt_value;
		}

		void write(T const& value) {
			if (this->rt_value == nullptr) {
				this->rt_value = this->to_rt.exchange(nullptr);
			}

			if (this->rt_value != nullptr) {
				*this->rt_value = value;
				this->rt_value = this->from_rt.exchange(this->rt_value);
			}
		}
	};
}
