#include "tepp/cheated_storage.h"

#if DEBUG_BUILD
namespace te::debug
{
	integer_t& typeCounter() {
		static integer_t counter = 0;
		return counter;
	}
}
#endif // DEBUG_BUILD

namespace te
{

}