#include "tepp/cstring_view.h"

#include <cassert>

namespace te
{
}

bool operator==(te::cstring_view left, std::string const& right) {
	return left.string_view() == right;
}