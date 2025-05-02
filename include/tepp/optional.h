#pragma once

#include <optional>

#include "tepp/nullopt.h"

#define UNWRAP_OPTIONAL(X, Y, Z) \
	if (auto Y##optional123456789 = Z) \
		for (X Y : Y##optional123456789)

namespace te
{
	template<class T>
	struct optional
	{
		std::optional<T> data{};

		operator bool() const {
			return this->has_value();
		}

		bool has_value() const {
			return this->data.has_value();
		}

		T* begin() {
			if (data.has_value()) {
				return &data.value();
			}
			else {
				return nullptr;
			}
		}

		T* end() {
			if (data.has_value()) {
				return &data.value() + 1;
			}
			else {
				return nullptr;
			}
		}

		T const* begin() const {
			if (data.has_value()) {
				return &data.value();
			}
			else {
				return nullptr;
			}
		}

		T const* end() const {
			if (data.has_value()) {
				return &data.value() + 1;
			}
			else {
				return nullptr;
			}
		}

		optional(optional const& other)
		    : data(other.data) {
		}

		optional& operator=(optional const& other) {
			this->data = other.data;
			return *this;
		}

		optional(optional&& other)
		    : data(std::move(other.data)) {
		}

		optional& operator=(optional&& other) {
			this->data = std::move(other.data);
			return *this;
		}

		optional(nullopt_t)
		    : data(std::nullopt) {
		}

		template<class... Args>
		optional(Args&&... args)
		    : data(std::forward<Args>(args)...) {
		}
	};

	template<class T, class F>
	inline bool optional_equal_with(std::optional<T> const& left, std::optional<T> const& right, F&& cmp) {
		if (left.has_value() != right.has_value()) {
			return false;
		}
		if (!left.has_value()) {
			return true;
		}
		return cmp(left.value(), right.value());
	}
}
