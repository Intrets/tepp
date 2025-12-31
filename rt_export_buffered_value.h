#pragma once

#include <array>
#include <atomic>

#include <tepp/assert.h>

namespace te
{
	template<class T>
	struct rt_export_buffered_value
	{
		std::array<T, 3> buffers{};

		T* rt_value = &buffers[0];

		std::atomic<T*> in = &buffers[1];
		std::atomic<T*> out = nullptr;

		T* non_rt_value = &buffers[2];

		T& read() {
			auto newValue = this->out.exchange(nullptr);

			if (newValue != nullptr) {
				[[maybe_unused]]
				auto old = this->out.exchange(this->non_rt_value);
				tassert(old == nullptr);
				this->non_rt_value = newValue;
			}

			return *this->non_rt_value;
		}

		void write(T const& value) {
			if (this->rt_value == nullptr) {
				this->rt_value = this->out.exchange(nullptr);
			}

			if (this->rt_value != nullptr) {
				*this->rt_value = value;
				this->rt_value = this->out.exchange(this->rt_value);
			}
		}
	};
}
