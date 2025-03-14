#pragma once

#include <atomic>
#include <cassert>
#include <source_location>

#ifdef OS_WIN
#include <intrin.h>
#define BREAK __debugbreak()
#elif defined(OS_UNIX)
#define BREAK raise(SIGTRAP)
#else
#error platform specific debug break statement not defined
#endif

bool isDebuggerPresent();
bool waitForDebugger(std::source_location sourceLocation);

#define DO_BREAK \
	if (isDebuggerPresent() || waitForDebugger(std::source_location::current())) { \
		BREAK; \
	}

#ifdef DEBUG_BUILD
#define tassert(B, ...) \
	do { \
		if (!(B)) [[unlikely]] { \
			DO_BREAK; \
		} \
	} while (0)
#else
#define tassert(...) ((void)0)
#endif
