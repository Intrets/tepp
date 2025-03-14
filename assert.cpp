#include "tepp/assert.h"

#include <iostream>

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

bool waitForDebugger(std::source_location sourceLocation) {
	auto message = std::format("Hit assert at {}({}:{}) `{}`", sourceLocation.file_name(), sourceLocation.line(), sourceLocation.column(), sourceLocation.function_name());
#ifdef OS_WIN

	PlaySoundW((LPCWSTR)SND_ALIAS_SYSTEMEXCLAMATION, NULL, SND_ALIAS_ID | SND_ASYNC);

	auto fullMessage = std::format("{}\n\nCancel to abort, try again to debug.", message);
	while (true) {
		auto result = MessageBox(0, fullMessage.c_str(), "Break", MB_CANCELTRYCONTINUE);

		switch (result) {
			case IDCANCEL:
				std::abort();
			case IDTRYAGAIN:
				if (IsDebuggerPresent()) {
					return true;
				}
				break;
			case IDCONTINUE:
				return false;
			default:
				break;
		}
	}

#else
	std::cout << message << "\n";
	std::cout << "Attach debugger and press enter\n";
	while (true) {
		std::cin.ignore();

		if (IsDebuggerPresent()) {
			BREAK;
			break;
		}
	}
#endif
}
