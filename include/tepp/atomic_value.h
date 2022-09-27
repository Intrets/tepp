// tepp - A template library for C++
// Copyright (C) 2022 intrets

#pragma once

#include <atomic>

namespace te
{
	// wrapper around std::atomic with only relaxed memory access
	template<class T>
		requires (std::atomic<T>::is_always_lock_free)
	struct atomic_value
	{
		std::atomic<T> value;

		T load() {
			return this->value.load(std::memory_order_relaxed);
		}

		void store(T v) {
			this->value.store(v, std::memory_order_relaxed);
		}
	};
}
