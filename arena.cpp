#include "tepp/arena.h"

#include <utility>

namespace te
{
	void arena::reset() {
		this->ptr = reinterpret_cast<char*>(this->data);
		this->remaining = this->size;
	}

	arena::arena(arena&& other)
	    : data(other.data),
	      ptr(other.ptr),
	      remaining(other.remaining),
	      size(other.size) {
		other.data = nullptr;
		other.ptr = nullptr;
		other.remaining = 0;
		other.size = 0;
	}

	arena& arena::operator=(arena&& other) {
		if (this->data != nullptr) {
			free(this->data);
		}

		this->data = other.data;
		this->ptr = other.ptr;
		this->remaining = other.remaining;
		this->size = other.size;

		other.data = nullptr;
		other.ptr = nullptr;
		other.remaining = 0;
		other.size = 0;

		return *this;
	}

	arena::arena(B size)
	    : arena(size.get()) {
	}

	arena::arena(KB size)
	    : arena(size.get()) {
	}

	arena::arena(MB size)
	    : arena(size.get()) {
	}

	arena::arena(integer_t size_) {
		this->data = malloc(size_);
		this->ptr = reinterpret_cast<char*>(this->data);

		if (this->data == nullptr) {
			std::abort();
		}

		this->size = static_cast<size_t>(size_);
		this->remaining = this->size;
	}

	arena::~arena() {
		free(this->data);
	}

	std::optional<integer_t> arena::checkSize() {
		if (this->capacityRequest != 0) {
			auto request = std::max(1024_i, this->capacityRequest * 2);
			this->capacityRequest = 0;
			return request;
		}
		else {
			return std::nullopt;
		}
	}
}