#include "tepp/name_thread.h"

#include "tepp/assert.h"

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

namespace te
{
	void name_thread(std::string_view name) {
#ifdef WIN32
		std::wstring wName;
		wName.resize(name.size() + 1);
		size_t convertedCount = 0;
		if (mbstowcs_s(&convertedCount, wName.data(), wName.size(), name.data(), _TRUNCATE) == 0) {
			SetThreadDescription(GetCurrentThread(), wName.data());
		}
		else {
			tassert(0);
		}
#endif
	}
}
