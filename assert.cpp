#include "tepp/assert.h"

#include <fstream>
#include <iostream>

#include <version>
#ifdef __cpp_lib_stacktrace
#include <stacktrace>
#endif

#ifdef OS_WIN
#include <Windows.h>

bool isDebuggerPresent() {
	return IsDebuggerPresent();
}
#else
#include <signal.h>
#include <sys/ptrace.h>

bool isDebuggerPresent() {
	std::ifstream proc_self_status_f("/proc/self/status");
	if (!proc_self_status_f) {
		return false;
	}

	std::array<char, 256> buffer_{ '\0' };
	proc_self_status_f.read(&buffer_[0], buffer_.size());

	std::string_view buffer(buffer_.data(), buffer_.size());
	auto index = buffer.find("TracerPid:\t");
	if (index == buffer.npos) {
		return false;
	}

	return buffer[index + 11] != '0';
}
#endif

bool waitForDebugger(std::source_location sourceLocation) {
	auto message = std::format("Hit assert at {}({}:{}) `{}`", sourceLocation.file_name(), sourceLocation.line(), sourceLocation.column(), sourceLocation.function_name());
#ifdef __cpp_lib_stacktrace
	std::format_to(std::back_inserter(message), "\nstacktrace:\n{}", std::stacktrace::current());
#endif

#ifdef OS_WIN
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

		if (isDebuggerPresent()) {
			BREAK;
			break;
		}
	}

	return true;
#endif
}
