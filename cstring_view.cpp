#include "tepp/cstring_view.h"

namespace te
{
}

bool operator==(te::cstring_view left, std::string const& right) {
	return left.string_view() == right;
}

te::cstring_view operator""_csv(char const* text, size_t size) {
	return te::cstring_view(text, size);
}