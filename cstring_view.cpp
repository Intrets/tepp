#include "tepp/cstring_view.h"

#include <cassert>

namespace te
{
	cstring_view::cstring_view(char const* str, size_t size_)
	    : data(str),
	      size(size_) {
		assert(str[size_] == '\0');
	}
}