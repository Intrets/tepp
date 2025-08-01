#pragma once

#include <optional>

#include "tepp/assert.h"
#include "tepp/nullopt.h"

namespace te
{
	template<class T>
	struct optional_ref
	{
		using value_type = T;

		T* ptr = nullptr;

		T& value() const {
			tassert(ptr != nullptr);
			return *ptr;
		}

		T& operator*() const {
			return this->value();
		}

		T* operator->() const {
			return &this->value();
		}

		bool has_value() const {
			return ptr != nullptr;
		}

		bool has_no_value() const {
			return ptr == nullptr;
		}

		void reset() {
			this->ptr = nullptr;
		}

		operator bool() const {
			return this->has_value();
		}

		T* begin() {
			if (this->has_value()) {
				return this->ptr;
			}
			else {
				return nullptr;
			}
		}

		T* end() {
			if (this->has_value()) {
				return this->ptr + 1;
			}
			else {
				return nullptr;
			}
		}

		T const* begin() const {
			if (this->has_value()) {
				return this->ptr;
			}
			else {
				return nullptr;
			}
		}

		T const* end() const {
			if (this->has_value()) {
				return this->ptr + 1;
			}
			else {
				return nullptr;
			}
		}

		optional_ref& operator=(T& object) {
			this->ptr = &object;
			return *this;
		}

		optional_ref(std::optional<T>& object) {
			if (object.has_value()) {
				this->ptr = &object.value();
			}
		}

		optional_ref(T& object) {
			this->ptr = &object;
		}

		optional_ref(T* ptr_) {
			this->ptr = ptr_;
		}

		optional_ref& operator=(te::nullopt_t) {
			this->ptr = nullptr;

			return *this;
		}

		template<class F>
		auto transform(F&& f) {
			using Return = decltype(f(this->value()));
			if (this->has_value()) {
				return std::make_optional(f(this->value()));
			}
			else {
				return std::optional<Return>(std::nullopt);
			}
		}

		std::optional<T> to_optional() const {
			if (this->has_value()) {
				return std::make_optional(this->value());
			}
			else {
				return std::nullopt;
			}
		}

		operator optional_ref<T const>() {
			return optional_ref<T const>(*this->ptr);
		}

		optional_ref() = default;
		optional_ref(te::nullopt_t) {
		}
		~optional_ref() = default;
	};
}
