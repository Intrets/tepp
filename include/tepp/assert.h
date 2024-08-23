#pragma once

#ifdef OS_WIN
#include <intrin.h>
#define BREAK __debugbreak()
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