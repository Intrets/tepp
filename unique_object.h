#pragma once

#include "tepp/misc.h"

#include <utility>

namespace te
{
	template<class T>
	struct unique_object
	{
	private:
		T value;

	public:
		NO_COPY(unique_object);

		T* operator->() {
			return &this->get();
		}

		T const* operator->() const {
			return &this->get();
		}

		T& get() {
			return this->value;
		}

		T const& get() const {
			return this->value;
		}

		unique_object& operator=(unique_object&& other) {
			this->value = std::move(other.value);
			other.value = {};
			return *this;
		}

		unique_object(unique_object&& other)
		    : value(std::move(other.value)) {
			other.value = {};
		}

		template<class... Args>
		unique_object(Args&&... args)
		    : value(std::forward<Args>(args)...) {
		}

		unique_object() = default;
		~unique_object() = default;
	};
}
