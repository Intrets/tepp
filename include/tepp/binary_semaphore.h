#pragma once

#include "tepp/assert.h"
#include "tepp/misc.h"

#include <atomic>
#include <concepts>

namespace te
{
	enum class blocking
	{
		spin,
		wait
	};

	enum class users
	{
		one_consumer__one_producer
	};

	template<blocking blocking_type, users users_type>
	struct binary_semaphore
	{
		std::atomic<unsigned char> counter;

		void acquire() noexcept {
			if constexpr (blocking_type == blocking::spin) {
				while (this->counter.load(std::memory_order_relaxed) == 0) {
				}
			}
			else if constexpr (blocking_type == blocking::wait) {
				this->counter.wait(0, std::memory_order_relaxed);
			}
			this->counter.store(0);
		}

		void release() noexcept {
			this->counter.store(1);
			if constexpr (blocking_type == blocking::wait) {
				this->counter.notify_one();
			}
		}

		NO_COPY_MOVE(binary_semaphore);

		binary_semaphore(unsigned char initial)
		    : counter(initial) {
			tassert(initial == 0 || initial == 1);
			static_assert(users_type == users::one_consumer__one_producer);
		}
		~binary_semaphore() = default;
	};
}