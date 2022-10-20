#pragma once

#include "tepp/misc.h"

#include <atomic>
#include <cassert>
#include <concepts>

namespace te
{
	struct spin {};
	struct wait {};

	template<class type>
	struct binary_semaphore
	{
		std::atomic<unsigned char> counter;

		void acquire() noexcept {
			while (true) {
				auto v = this->counter.exchange(0, std::memory_order_acquire);

				if (v == 1) {
					return;
				}

				if constexpr (std::same_as<type, wait>) {
					this->counter.wait(0, std::memory_order_relaxed);
				}
			}
		}

		void release() noexcept {
			this->counter.store(1, std::memory_order_release);
			if constexpr (std::same_as<type, wait>) {
				this->counter.notify_one();
			}
		}

		NO_COPY_MOVE(binary_semaphore);

		binary_semaphore(unsigned char initial) : counter(initial) {
			assert(initial == 0 || initial == 1);
		}
		~binary_semaphore() = default;
	};
}