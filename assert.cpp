#include "tepp/assert.h"

#ifdef OS_WIN
#include <Windows.h>
bool isDebuggerPresent() {
	return IsDebuggerPresent();
}
#else
bool isDebuggerPresent() {
	return true;
}
#endif
