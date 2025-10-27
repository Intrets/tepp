#pragma once

#include "assert.h"
#include "tepp/misc.h"

namespace te
{
	template<class T>
	using futex = std::atomic<T>;
}

#ifdef OS_UNIX
#include <linux/futex.h> /* Definition of FUTEX_* constants */
#include <sys/syscall.h> /* Definition of SYS_* constants */

namespace te
{
	template<class T>
	using futex = std::atomic<T>;

	template<class T>
	struct futex2
	{
	private:
		static_assert(sizeof(T) <= 4);
		static_assert(std::is_trivially_copyable_v<T>);

		static uint32_t convertToValue(T value) {
			if constexpr (sizeof(T) == 4) {
				return std::bit_cast<uint32_t>(value);
			}
			else {
				uint32_t result = 0;
				std::memcpy(&result, &value, sizeof(value));
				return result;
			}
		}

		static T convertFromValue(uint32_t value) {
			if constexpr (sizeof(T) == 4) {
				return std::bit_cast<T>(value);
			}
			else {
				T result{};
				std::memcpy(&value, &result, sizeof(value));
				return result;
			}
		}

		uint32_t value;

	public:
		futex2()
		    : value(convertToValue({})) {
		}

		futex2(T t)
		    : value(convertToValue(t)) {
		}

		~futex2() = default;

		NO_COPY_MOVE(futex2);

		void wait(T t) {
			auto value_ = convertToValue(t);

			while (__atomic_load_n(&this->value, __ATOMIC_SEQ_CST) == value_) {
				auto res = syscall(SYS_futex, &this->value, FUTEX_WAIT_PRIVATE, value_, NULL);

				if (res == -1 && errno != EAGAIN) {
					tassert(0);
					std::abort();
				}
			}
		}

		void store(T t) {
			auto value_ = convertToValue(t);

			__atomic_store_n(&this->value, value_, __ATOMIC_SEQ_CST);
		}

		void notify_all() {
			auto res = syscall(SYS_futex, &this->value, FUTEX_WAKE_PRIVATE, INT_MAX);

			if (res == -1) {
				tassert(0);
				std::abort();
			}
		}
	};
}
#else
namespace te
{
	template<class T>
	using futex2 = futex<T>;
}
#endif
