#include "tepp/name_thread.h"

#include <cstring>

#include "tepp/assert.h"
#include "tepp/cstring_view.h"
#include "tepp/integers.h"

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#else
#include <sys/prctl.h>
#endif

namespace te
{
	void name_thread(te::cstring_view name) {
#ifdef WIN32
		std::wstring wName;
		wName.resize(name.size() + 1);
		size_t convertedCount = 0;
		if (mbstowcs_s(&convertedCount, wName.data(), wName.size(), name.getData(), _TRUNCATE) == 0) {
			SetThreadDescription(GetCurrentThread(), wName.data());
		}
		else {
			tassert(0);
		}
#else
		if (isize(name) > 15) {
			std::array<char, 16> truncatedName{};
			std::memcpy(truncatedName.data(), name.getData(), 7);
			truncatedName[7] = '~';
			std::memcpy(truncatedName.data() + 8, name.getData() + isize(name) - 7, 7);
			truncatedName[15] = '\0';

			prctl(PR_SET_NAME, truncatedName.data());
		}
		else {
			prctl(PR_SET_NAME, name.getData());
		}
#endif
	}
}
