#include "tepp/cstring_view.h"

#include <cassert>

namespace te
{
	bool cstring_view::empty() const {
		return this->size == 0;
	}

	cstring_view::cstring_view(std::string const& str)
	    : data(str.data()),
	      size(str.size()) {
	}

	cstring_view::cstring_view(char const* str, size_t size_)
	    : data(str),
	      size(size_) {
		assert(str[size_] == '\0');
	}

	cstring_view& cstring_view::set(std::string const& str) {
		this->data = str.data();
		this->size = str.size();

		return *this; 
	}
}