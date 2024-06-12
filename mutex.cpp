// tepp - a template library for c++
// Copyright (C) 2022 intrets

#include "tepp/mutex.h"

#if USE_WINDOWS
#include <Windows.h>
#endif

namespace te
{
#if USE_WINDOWS
	void mutex::lock() {
		EnterCriticalSection(&this->criticalSection.access<CRITICAL_SECTION>());
	}

	void mutex::unlock() {
		LeaveCriticalSection(&this->criticalSection.access<CRITICAL_SECTION>());
	}

	mutex::mutex() {
		this->criticalSection.init<CRITICAL_SECTION>();
		InitializeCriticalSection(&this->criticalSection.access<CRITICAL_SECTION>());
	}

	mutex::~mutex() {
		DeleteCriticalSection(&this->criticalSection.access<CRITICAL_SECTION>());
	}
#endif
}
