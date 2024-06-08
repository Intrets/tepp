#include "tepp/cheated_storage.h"

#if DEBUG_BUILD
namespace te::debug
{
	int64_t& typeCounter() {
		static int64_t counter = 0;
		return counter;
	}
}
#endif // DEBUG_BUILD

namespace te
{

}