#pragma once

#include <atomic>

#ifdef OS_WIN
#include <intrin.h>
#define BREAK __debugbreak()
#elifdef OS_UNIX
#define BREAK raise(SIGTRAP)
#else
#error platform specific debug break statement not defined
#endif

#ifdef DEBUG_BUILD
#define tassert(B, ...) \
	do { \
		if (!(B)) [[unlikely]] { \
			BREAK; \
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
				BREAK; \
			} \
		} \
	} while (0)
#else
#define tassertonce(...) ((void)0)
#endif