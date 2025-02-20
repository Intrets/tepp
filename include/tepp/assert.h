#pragma once

#include <atomic>
#include <cassert>

#ifdef OS_WIN
#include <intrin.h>
#define BREAK __debugbreak()
#elif defined(OS_UNIX)
#define BREAK raise(SIGTRAP)
#else
#error platform specific debug break statement not defined
#endif

bool isDebuggerPresent();

#define DO_BREAK \
	if (isDebuggerPresent()) { \
		BREAK; \
	} \
	else { \
		assert(0); \
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

#ifdef DEBUG_BUILD
#define tassertonce(B, ...) \
	do { \
		if (!(B)) [[unlikely]] { \
			static std::atomic<bool> flag = false; \
			if (!flag.exchange(true)) { \
				DO_BREAK; \
			} \
		} \
	} while (0)
#else
#define tassertonce(...) ((void)0)
#endif