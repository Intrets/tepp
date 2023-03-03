#include "tepp/cstring_view.h"

namespace te
{
	char const* cstring_view::getData() const {
		return this->data;
	}
}