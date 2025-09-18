// tepp - a template library for c++
// Copyright (C) 2022 intrets

#pragma once

#include "misc.h"

#if (defined(WIN32) || defined(_WIN32) || defined (_WIN64)|| defined(__WIN32__) || defined(__WINDOWS__)) && !defined(__CYGWIN__)
#define USE_WINDOWS 1
#endif

#if USE_WINDOWS
#include "cheated_storage.h"
#else
#include <mutex>
#endif

namespace te
{
#if USE_WINDOWS 
	struct mutex
	{
	public:
		void lock();
		void unlock();

		mutex();
		~mutex();

		NO_COPY_MOVE(mutex);

	private:
		cheated_storage<40, 8> criticalSection;
	};
#else
	using mutex = std::recursive_mutex;
#endif
	}
