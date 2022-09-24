#include "tepp/cheated_storage.h"

#if DEBUG_BUILD
namespace te::debug
{
	int& typeCounter() {
		static int counter = 0;
		return counter;
	}
}
#endif // DEBUG_BUILD

namespace te
{

}