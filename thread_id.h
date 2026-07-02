#pragma once

#include "tepp/misc.h"

namespace te
{
	enum class thread_types
	{
		uninitialized,
		real_time,
		bypassed,
		other,
	};

	static thread_types& thread_type() {
		thread_local thread_types value = thread_types::uninitialized;

		return value;
	}

	struct scoped_thread_type
	{
		thread_types old_type = thread_type();

		NO_COPY_MOVE(scoped_thread_type);

		scoped_thread_type() = delete;
		scoped_thread_type(thread_types type) {
			thread_type() = type;
		}

		~scoped_thread_type() {
			thread_type() = this->old_type;
		}
	};
}
