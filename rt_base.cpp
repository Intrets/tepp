#include "tepp/rt_base.h"

namespace te
{
	rt_aggregate_dynamic& rt_base::getAggregate() {
		return *this->aggregate;
	}
}
